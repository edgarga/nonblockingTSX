#include <iostream>
#include "list.h"
int main(int numberOfArguments, char* arguments[]){
    List list;
    list.print();

    std::cout << "inserting 1" << std::endl;
    list.insert(1);
    list.insert(5);
    list.insert(2);
    list.print();

    std::cout << "deleting 1" << std::endl;
    list.del(5);
    list.print();
    return 0;
}

