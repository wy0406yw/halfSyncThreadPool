#pragma once
#include<list>
#include<thread>
#include<functional>
#include<memory>
#include<atomic>
#include"SyncQueue.hpp"

const int MaxTaskCount = 100;
class ThreadPool
{
    //Task是封装的函数指针，函数签名void（）
    using Task = std::function<void()>;
private:
    //使用智能指针封装的线程，再封装为std::list
    std::list<std::shared_ptr<std::thread>> m_threadgroup;
    //使用同步队列
    SyncQueue<Task> m_queue;
    //原子布尔类型
    atomic_bool m_running;
    std::once_flag m_flag;

    void RunInThread()
    {
        while (m_running)
        {
            std::list<Task> list;
            m_queue.Take(list);

            for (auto& task : list)
            {
                if (!m_running) return;
                task();//执行
            }
        }

    }

    void StopThreadGroup()
    {
        m_queue.Stop();
        m_running = false;

        for (auto& thread : m_threadgroup)
        {
            if (thread) thread->join();
        }

        m_threadgroup.clear();
    }



    void Start(int numThreads)
    {
        m_running = true;
        //创建线程组
        for (int i = 0; i < numThreads; i++)
        {
            //std::thread a(&ThreadPool::RunInThread,this);
            //创建num_threads个匿名线程运行RunInThread
            m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
        }
    }

public:
    //hardware_concurrency()是一个 C++ 11 库函数，用于返回当前硬件支持的并发线程数。
    ThreadPool(int numThreads = std::thread::hardware_concurrency()) : m_queue(MaxTaskCount)
    {
        Start(numThreads);
    }

    ~ThreadPool(void)
    {
        Stop();
        //主动停止线程池
    }

    void Stop()
    {
        //多线程情况下只调用一次停止
        std::call_once(m_flag, [this] {StopThreadGroup(); });
    }

    void AddTask(Task&& task)
    {
        m_queue.Put(task);
    }

    void AddTask(const Task& task)
    {
        m_queue.Put(task);
    }

};