//
// Created by edgar on 10.01.18.
//

#include "node.h"

Node::Node() {
    this->next = nullptr;
    this->value = -10;
    this->markedToDelete = false;
}

Node::Node(int val) {
    this->value = val;
    this->next = nullptr;
    this->markedToDelete = false;
}

Node::~Node() {}