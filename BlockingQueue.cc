#include "BlockingQueue.h"

template <typename T>
void pusher(BlockingQueue<T>& q){
	for (int i = 0 ; i < 10000; ++i)
		q.push(i);
}

template <typename T>
T taker(BlockingQueue<T>& q){
	for (int i = 0 ; i < 20000; ++i)
		q.front();
}



/*
 *	运行结果应该是阻塞住
 *	因为存了10000个元素却取了20000个，取无可取的时候应该阻塞住
 * */
int main(){
	BlockingQueue<int> q;
	
	std::thread t2(taker<int>, std::ref(q));
	std::thread t1(pusher<int>, std::ref(q));
	
	t1.join();
	t2.join();
	return 0;
}
