#include <iostream>
#include "LinkedList.h"
#include <thread>
#include <thread>
#include <chrono>
#include <typeinfo>
class someClass{
	private: 
		int value;
		someClass* partPointer;
	public:
		someClass(){

		}

		void setValue(int value){this->value = value;}
		int getValue(){return value;}
};

void a(LinkedList &list){
	std::cout << "A: try to lock. type: "  << std::endl;
	list.head->next->doSomething();
	list.head->next->checkLock();
	//std::unique_lock<std::mutex> lockS(list.head->next->mtex);
	
	//.lock();
	//lock.lock();
	std::cout << "A: locked" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));
}

void b(LinkedList &list){
	std::cout << "B: tryLock(): ";
	std::unique_lock<std::mutex> try_lock(list.head->next->mtex);
	
	while(!try_lock.try_lock()){
		std::cout << "waiting" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	//if(list.head->next->mtex.try_lock())
}

int main(int numberOfArguments, char* arguments[]){
	// someClass* some = new someClass();// = new someClass();
	// someClass some2;
	// some->setValue(12);

	// std::cout << "hello world " << some->getValue() << std::endl;
	
	// some2.setValue(113);
	// std::cout << "hello world " << some2.getValue() << std::endl;

	LinkedList list;
	if(list.head->next == list.tail){
		std::cout << "true " << std::endl;
	}
	list.insert(5);
	list.insert(10);
	list.insert(1009);
	list.print();
	std::cout << "pop(): " << list.pop() << std::endl;
	list.print();
	//list.head->next->mtex.lock();
	//list.head->next->mtex.lock();

	std::thread ta(a, std::ref(list));
	//std::thread tb(b, std::ref(list));

	ta.join();
	//tb.join();

	return 0;
}