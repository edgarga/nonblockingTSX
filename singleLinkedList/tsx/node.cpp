//
// Created by edgar on 10.01.18.
//

#include "node.h"

Node::Node(int key, bool *ptr){
    this->key = key;
    this->marked = ptr;
    this->next = nullptr;
}

Node::~Node(){

}