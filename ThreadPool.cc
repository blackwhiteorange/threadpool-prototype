#include "ThreadPool.h"

#include <unistd.h>
#include <time.h>


ThreadPool::ThreadPool(int initCount){
	assert(initCount > 0);
	isRunning = true;

	for (int i = 0 ; i < initCount; ++i){
		threads_.push_back(std::thread(
			[this](){
				while(true){
					voidFunc task;
					// 获取任务必须加锁，执行任务不需要加锁，除非任务和任务之间共享数据，不过这里不会
					// 大括号，限制unique_lock的作用域，否则lck不会及时释放mtx
					// 在这个场景下，mtx是否被释放，直接影响到了threadpool在析构时对于线程的处理
					{
						std::unique_lock<std::mutex> lck(mtx);
						
						while (isRunning && tasks_.empty()){
							cv.wait(lck);
						}
						if (!isRunning && tasks_.empty()){
							std::cout<<std::setbase(16)<<std::this_thread::get_id()<<" returns"<<std::endl;
							return;
						}
						task = std::move(tasks_.front());
						tasks_.pop();
					}
					task();		
					}
				}
			)
		);
	}

	for (auto& t: threads_){
		std::cout<<std::setbase(16)<<t.get_id()<<" "<<t.joinable()<<std::endl;
	}
}


ThreadPool::~ThreadPool(){
	{	
		std::unique_lock<std::mutex> lck(mtx);
		isRunning = false;
	}
	while (!tasks_.empty()){
		tasks_.pop();
	}
	//lck.unlock();
	cv.notify_all();
	for (std::thread& t: threads_){
		std::cout<<std::setbase(16)<<t.get_id()<<" set to join"<<std::endl;
		t.join();
	}
}



void ThreadPool::start(){
}


void ThreadPool::stop(){	
}

void ThreadPool::addTask(voidFunc func){
	std::unique_lock<std::mutex> lck(mtx);
	if (!isRunning){
		throw std::runtime_error("enqueue on stopping threadpool");
	}
	tasks_.push(func);
	cv.notify_one();
}


void printNum(){
	std::cout<<std::setbase(16)<<"printNum from "<<std::this_thread::get_id()<<std::endl;
}

#ifdef THREADPOOL
int main(){
	time_t start, end;
	start = clock();
	ThreadPool pool(6);
	for (int i = 0 ; i < 12000; ++i)
		pool.addTask(printNum);
	sleep(1);
	end = clock();
	std::cout<<"thread pool: "<<double(end - start)/CLOCKS_PER_SEC<<std::endl;
	return 0;
}
#endif
