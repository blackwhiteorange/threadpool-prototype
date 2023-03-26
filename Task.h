
#include <functional>
#include <iostream>


class Task{
	public:
		Task(std::function<void()> func);
		void execute();
	private:
		std::function<void()> func;
};
