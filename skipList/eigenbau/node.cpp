//
// Created by edgar on 26.02.18.
//

#include <limits>
#include "node.h"

/**
 * Common Initializer for Constructors
 */
void Node::Init() {
    this->value = std::numeric_limits<int>::min();
    this->isHeadNode = false;
    this->isLimit = false;
    this->next = nullptr;
    this->root = nullptr;
    this->down = nullptr;
    this->limitNextUp = nullptr;
    this->level = -1;
}

Node::Node(int key) {
    Init();
    this->value = key;
}

Node::Node(bool isLimitingNode, bool isHeadNode) {
    Init();
    this->isLimit = isLimitingNode;
    this->isHeadNode = isHeadNode;

    if(!isHeadNode)
        this->value = std::numeric_limits<int>::max();
}

Node::~Node() {}

