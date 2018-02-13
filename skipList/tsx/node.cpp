//
// Created by edgar on 12.02.18.
//

#include <limits>
#include "node.h"

void Node::Init(int value) {
    this->value = value;
    this->successor = nullptr;
    this->down = nullptr;
    this->towerRoot = nullptr;
    this->backLink = nullptr;
    this->limitDown = nullptr;
    this->limitUp = nullptr;
}

Node::Node(int value){
    this->Init(value);
}

Node::Node(bool isLimit, bool isHead): isLimitingNode(isLimit), isHeadNode(isHead) {

    if(isHead)
        Init(std::numeric_limits<int>::min());
    else
        Init(std::numeric_limits<int>::max());
}

Node::~Node() {}

