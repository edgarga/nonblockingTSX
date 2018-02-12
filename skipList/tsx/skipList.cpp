//
// Created by edgar on 12.02.18.
//

#include <limits>
#include "skipList.h"


SkipList::SkipList(int levelCount) : maxLevel(levelCount) {
    Node *lowerHead = new Node(true, true);
    Node *lowerTail = new Node(true, false);
    this->headRoot = lowerHead;
    this->tailRoot = lowerTail;
    lowerHead->successor = lowerTail;

    for (int i = 0; i < this->maxLevel; i++) {
        Node *newHead = new Node(true, true);
        Node *newTail = new Node(true, false);
        newHead->successor = newTail;

        newHead->limitDown = lowerHead;
        newTail->limitDown = lowerTail;

        lowerHead->limitUp = newHead;
        lowerTail->limitUp = newTail;

        lowerHead = newHead;
        lowerTail = newTail;
    }

    lowerHead->limitUp = lowerHead;
    lowerTail->limitUp = lowerTail;

}

SkipList::~SkipList() {

}

bool SkipList::isMarked(size_t ptr) {
    return (ptr & 3) != 0;
}

bool SkipList::isMarked(Node *node) {
    return this->isMarked((size_t) node);
}

bool SkipList::isMarked(Node *node, int position) {
    return this->isMarked((size_t) node, position);
}

bool SkipList::isMarked(size_t ptr, int position) {
    return (ptr & (1 << position)) != 0;
}

Node *SkipList::getUnmarkedPtr(Node *node) {
    return this->getUnmarkedPtr((size_t) node);
}

Node *SkipList::getUnmarkedPtr(size_t ptr) {
    size_t unmarkedPtr = ptr & std::numeric_limits<size_t>::max() - 3;
    return (Node *) unmarkedPtr;
}

Node *SkipList::getUnmarkedPtr(Node *node, int unmarkingPosition) {
    return this->getUnmarkedPtr((size_t) node, unmarkingPosition);
}

Node *SkipList::getUnmarkedPtr(size_t ptr, int unmarkingPosition) {
    size_t unmarkedPtr = ptr | (std::numeric_limits<size_t>::max() - (1 << unmarkingPosition));
    return (Node *) unmarkedPtr;
}

Node *SkipList::getMarkedPtr(Node *node, int markingPosition) {
    return this->getMarkedPtr((size_t) node, markingPosition);
}

Node *SkipList::getMarkedPtr(size_t ptr, int markingPosition) {
    size_t markedPtr = ptr | (1 << markingPosition);
    return (Node *) markedPtr;
}

bool SkipList::helpMarked(Node *prevNode, Node *delNode) {
    Node *nextUnmarked = this->getUnmarkedPtr(delNode->successor);
    return prevNode->successor.compare_exchange_weak(delNode, nextUnmarked);
}

bool SkipList::helpFlagged(Node *prevNode, Node *delNode) {
    delNode->backLink = prevNode;
    if (!this->isMarked(delNode->successor, 1))
        this->tryMark(delNode);
    this->helpMarked(prevNode, delNode);
}

bool SkipList::tryMark(Node *node) {
    do {
        Node *nextNode = this->getUnmarkedPtr(node->successor);
        node->successor.compare_exchange_weak(nextNode, this->getMarkedPtr(nextNode, 1));
        if (!this->isMarked(node->successor, 1) && this->isMarked(node->successor, 0))
            this->helpFlagged(node, nextNode->successor);
    }while (!this->isMarked(node->successor, 1));
}

