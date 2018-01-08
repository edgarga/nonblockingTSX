//
// Created by edgar on 07.01.18.
//
#include <iostream>
#include "node.h"

Node::Node() {
    this->value = -1;
    this->next = nullptr;
    this->deleted = false;
}

Node::Node(int value) {
    this->value = value;
    this->next = nullptr;
    this->deleted = false;
}

bool Node::setNextProtected(Node *newNext) {

}

Node::~Node() {
    std::cout << "deleting node!: " << this->value << std::endl;
}
