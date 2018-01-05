#include <iostream>
#include "node.h"

Node::Node(){
    this->key = -1;
    this->next = nullptr;
    this->markedToDelele = false;
}

Node::Node(int key){
    this->key = key;
    this->markedToDelele = false;
}

Node::~Node(){
    std::cout << "deleting Node: " << this->key << " " << std::endl;
}