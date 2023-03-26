/*
 *	BlockingQueue是最常见的一个模型
 *	是一个支持多线程进行存，取的队列
 */


#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <iostream>

template <typename T>
class BlockingQueue{
	public:
		// 向队尾添加一个元素
		void push(T t);

		// 这个自定义的front方法既取队首元素，也会弹出队首元素
		T front();

		int size();
		bool empty();
	private:
		std::mutex mtx;
		std::condition_variable empty_cv;

		int q_size;

		std::queue<T> q;
};


template <typename T>
void BlockingQueue<T>::push(T t){
	std::unique_lock<std::mutex> lck(mtx);
	q.push(t);
	q_size ++;
	//std::cout<<"element pushed in, the size is now "<<q_size<<std::endl;
	empty_cv.notify_all();
}

template <typename T>
T BlockingQueue<T>::front(){
	std::unique_lock<std::mutex> lck(mtx);
	empty_cv.wait(lck, [&](){return this->q_size > 0;});
	q_size--;
	T t = q.front();
	q.pop();
	//std::cout<<"element popped out, the size is now "<<q_size<<std::endl;
	empty_cv.notify_all();
	return t;
}

template <typename T>
int BlockingQueue<T>::size(){
	std::unique_lock<std::mutex> lck(mtx);
	return q.size();
}

template <typename T>
bool BlockingQueue<T>::empty(){
	std::unique_lock<std::mutex> lck(mtx);
	return q.empty();
}

