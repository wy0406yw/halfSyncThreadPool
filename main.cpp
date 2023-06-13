#include<iostream>
#include"ThreadPool.hpp"
#include"SyncQueue.hpp"

void TestThdPool()
{
	ThreadPool pool;
	std::thread thd1([&pool] {
		for (int i = 0; i < 10; i++)
		{
			auto thdId = this_thread::get_id();
			pool.AddTask([thdId] {
				std::cout << "同步层的线程1的线程ID：" << thdId << "\n";
				});
		} //for

		});

	std::thread thd2([&pool] {
		for (int i = 0; i < 10; i++)
		{
			auto thdId = this_thread::get_id();
			pool.AddTask([thdId] {
				std::cout << "同步层的线程2的线程ID：" << thdId << "\n";
				});
		} //for

		});

	this_thread::sleep_for(std::chrono::seconds(2));
	
	pool.Stop();
	thd1.join();
	thd2.join();
	

}


int main()
{
	TestThdPool();
	return 0;
}