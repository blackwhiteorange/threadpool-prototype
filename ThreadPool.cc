#include "ThreadPool.h"

#include <unistd.h>

ThreadPool::ThreadPool(int initCount){
	assert(initCount > 0);
	isRunning = true;

	for (int i = 0 ; i < initCount; ++i){
		threads_.push_back(std::thread(
			[&](){
				while(isRunning){
					
					std::unique_lock<std::mutex> lck(mtx);
					cv.wait(lck, [&](){return !tasks_.empty() || !isRunning;});
					if (!isRunning && tasks_.empty()){
						std::cout<<std::setbase(16)<<std::this_thread::get_id()<<" returns"<<std::endl;
						return;
					}
					/*
					std::cout<<std::setbase(16)<<"thread "<<std::this_thread::get_id()<<": "<<tasks_.size()<<std::endl;
					std::this_thread::sleep_for(std::chrono::milliseconds(2));
					*/

					voidFunc task = tasks_.front();
					tasks_.pop();
					task();
				
				}
			}
		)
		);
	}

	for (auto& t: threads_){
		std::cout<<std::setbase(16)<<t.get_id()<<std::endl;
	}
}


ThreadPool::~ThreadPool(){
	std::unique_lock<std::mutex> lck(mtx);
	std::cout<<"shutting down threadpool"<<std::endl;
	stop();
}



void ThreadPool::start(){
	isRunning = true;
}


void ThreadPool::stop(){	
	isRunning = false;
	cv.notify_all();
	for (auto& t: threads_){
		if (t.joinable()){
			t.join();
			std::cout<<std::setbase(16)<<"thread "<<t.get_id()<<" set to join"<<std::endl;
		}
	}
}

void ThreadPool::addTask(voidFunc func){
	tasks_.push(func);
	cv.notify_all();
}


void printNum(){
	std::cout<<std::setbase(16)<<"printNum from "<<std::this_thread::get_id()<<std::endl;
}

#ifdef THREADPOOL
int main(){
	ThreadPool pool(5);
	pool.addTask(printNum);
	sleep(1);
	return 0;
}
#endif
