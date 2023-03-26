#include "Task.h"


Task::Task(std::function<void()> func){
	this->func = func;
}


void Task::execute(){
}



#ifdef THREADPOOL

int main(){
	std::function<void()> f = print;
	f();
	return 0;

}

#endif
