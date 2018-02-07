//
// Created by edgar on 10.01.18.
//

#include <iostream>
#include "node.h"

Node::Node(int key){
    this->key = key;
    this->next = nullptr;
}

Node::~Node(){
//    std::cout << "deleting: " << this->key << std::endl;
}