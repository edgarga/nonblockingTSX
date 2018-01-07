#include <iostream>
#include "node.h"

Node::Node(){
    this->key = -1;
    this->next = nullptr;
    this->markedToDelete = false;
}

Node::Node(int key){
    this->key = key;
    this->markedToDelete = false;
}

Node::~Node(){
    std::cout << "deleting Node: " << this->key << " " << std::endl;
}