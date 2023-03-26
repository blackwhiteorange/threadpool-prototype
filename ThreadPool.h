#define THREADPOOL

#include <thread>
#include <vector>
#include <queue>
#include <iostream>

#include <condition_variable>
#include <mutex>

#include <assert.h>
#include <iomanip>
#include <chrono>


#include "Task.h"
#include "BlockingQueue.h"
/*
 *	线程池中的每一个线程都应该运行如下的函数
 *	
 *	while(1)
 *	{
 *		等待task队列内有任务
 *		取出任务，运行任务
 *	}
 *
 *	所以任务队列每一次在enqueue一个任务时，都要通知所有的线程，这正对应了notify_all()方法
 *	
 * */


typedef std::function<void()> voidFunc;

class ThreadPool{
	public:
		ThreadPool(int initCount);
		~ThreadPool();


		void start();
		void stop();

		void addTask(voidFunc func);

	private:
		std::vector<std::thread> threads_;
		//BlockingQueue<voidFunc> tasks_;
		std::queue<voidFunc> tasks_;

		bool isRunning;

		std::condition_variable cv;
		std::mutex mtx;
};
