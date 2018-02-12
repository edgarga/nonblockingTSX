//
// Created by edgar on 12.02.18.
//

#include <limits>
#include "node.h"

Node::Node(int value): value(value){
    this->successor = nullptr;
    this->down = nullptr;
    this->towerRoot = nullptr;
    this->backLink = nullptr;
    this->limitDown = nullptr;
    this->limitUp = nullptr;

}

Node::Node(bool isLimit, bool isHead): isLimitingNode(isLimit), isHeadNode(isHead) {
    if(this->isHeadNode)
        Node(std::numeric_limits<int>::min());
    else
        Node(std::numeric_limits<int>::max());
}

Node::~Node() {}

