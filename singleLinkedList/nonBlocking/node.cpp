#include <iostream>
#include "node.h"

//Node::Node(){
//    this->key = -1;
//    this->next = nullptr;
//    this->marked = new int(1);
//    this->markedToDelete = false;
//}

Node::Node(int key){
    this->key = key;
    this->next = nullptr;
}

Node::~Node(){
    std::cout << "deleting Node: " << this->key << " " << std::endl;
}