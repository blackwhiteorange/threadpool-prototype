#include "ThreadPool.h"

#include <unistd.h>

ThreadPool::ThreadPool(int initCount){
	assert(initCount > 0);
	isRunning = true;

	for (int i = 0 ; i < initCount; ++i){
		threads_.push_back(std::thread(
			[this](){
				while(true){
					std::unique_lock<std::mutex> lck(mtx);
					//cv.wait(lck, [this](){std::cout<<!tasks_.empty()<<" "<<!isRunning<<std::endl; return !tasks_.empty() || !isRunning;});
					while (isRunning && tasks_.empty()){
						std::cout<<isRunning<<" "<<tasks_.empty()<<std::endl;
						cv.wait(lck);
					}
					
					if (!isRunning && tasks_.empty()){
						std::cout<<std::setbase(16)<<std::this_thread::get_id()<<" returns"<<std::endl;
						return;
					}
					voidFunc task = std::move(tasks_.front());
					tasks_.pop();
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
	std::unique_lock<std::mutex> lck(mtx);
	isRunning = false;
	while (!tasks_.empty()){
		tasks_.pop();
	}
	lck.unlock();
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
	ThreadPool pool(6);
	pool.addTask(printNum);
	pool.addTask(printNum);
	sleep(1);
	return 0;
}
#endif
