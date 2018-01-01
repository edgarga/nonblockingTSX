#include <iostream>
#include "ListLinked.h"
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

int main(int numberOfArguments, char* arguments[]){
	// someClass* some = new someClass();// = new someClass();
	// someClass some2;
	// some->setValue(12);

	// std::cout << "hello world " << some->getValue() << std::endl;
	
	// some2.setValue(113);
	// std::cout << "hello world " << some2.getValue() << std::endl;

	ListLinked *list = new ListLinked();
	if(list->head->next == list->tail){
		std::cout << "true " << std::endl;
	}
	list->insert(5);
	list->insert(10);
	list->insert(1009);
	list->print();
	std::cout << "pop(): " << list->pop() << std::endl;
	list->print();

	return 0;
}