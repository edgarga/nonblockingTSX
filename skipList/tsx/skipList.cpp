//
// Created by edgar on 12.02.18.
//

#include <limits>
#include <iostream>
#include "skipList.h"

#define SKL_IN 1 << 1
#define SKL_DELETED 1 << 2
#define SKL_DUPLICATE_KEY 1 << 3

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

bool SkipList::isMarkedOnPosition(Node *node, int position) {
    return this->isMarkedOnPosition((size_t) node, position);
}

bool SkipList::isMarkedOnPosition(size_t ptr, int position) {
    return (ptr & (1 << position)) != 0;
}

Node *SkipList::getUnmarkedPtr(Node *node) {
    return this->getUnmarkedPtr((size_t) node);
}

Node *SkipList::getUnmarkedPtr(size_t ptr) {
    size_t unmarkedPtr = ptr & (std::numeric_limits<size_t>::max() - 15);
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

bool SkipList::insert(int key) {
    Node *prevNode, *nextNode;
    prevNode = searchToLevel(key, 1, &nextNode);

    if (prevNode->value == key)
        return false;

    Node *newRootNode = new Node(key);
    newRootNode->towerRoot = newRootNode;
    Node *newNode = newRootNode;
    int towerHeight = ((rand() % 2) * (rand() % (this->maxLevel - 2))) + 1;

    int currentLevel = 1;
    while (true) {
        Node *result;
        prevNode = insertNode(newNode, prevNode, nextNode, &result);
        if (isDuplicateKey(result) && currentLevel == 1)
            return false;
        if (isMarkedOnPosition(newRootNode->successor, 1)) {
            if ((getUnmarkedPtr(result) == newNode) && (newNode != newRootNode))
                removeNode(prevNode, newNode);
            return true;
        }

        currentLevel++;

        if (currentLevel == towerHeight + 1)
            return true;

        Node *lastNode = newNode;
        newNode = new Node(key);
        newNode->down = lastNode;
        newNode->towerRoot = newRootNode;
        prevNode = searchToLevel(key, currentLevel, &nextNode);
    }


}

Node *SkipList::insertNode(Node *newNode, Node *prevNode, Node *nextNode, Node **result) {
    if (prevNode->value == newNode->value) {
        size_t resultSequence = SKL_DUPLICATE_KEY;
        (*result) = (Node *) resultSequence;
        return prevNode;
    }

    while (true) {
        Node *nextOfPrev = prevNode->successor;
        if (isMarkedOnPosition(nextOfPrev, 0))
            helpFlagged(prevNode, getUnmarkedPtr(nextOfPrev)->successor);
        else {
            newNode->successor = getUnmarkedPtr(nextNode);
            if (getUnmarkedPtr(prevNode)->successor.compare_exchange_weak(nextNode, newNode)) {
                (*result) = newNode;
                return prevNode;
            } else {
                if (isMarkedOnPosition(getUnmarkedPtr(prevNode)->successor, 0))
                    helpFlagged(prevNode, getUnmarkedPtr(getUnmarkedPtr(prevNode)->successor));
                while (isMarkedOnPosition(prevNode->successor, 1))
                    prevNode = getUnmarkedPtr(prevNode)->backLink;
            }

        }
        prevNode = searchRight(newNode->value, prevNode, &newNode);
        if(getUnmarkedPtr(prevNode)->value == newNode->value){
            size_t resultSequence = SKL_DUPLICATE_KEY;
            (*result) = (Node *) resultSequence;
            return prevNode;
        }
    }


}

bool SkipList::helpMarked(Node *prevNode, Node *delNode) {
    Node *nextUnmarked = this->getUnmarkedPtr(delNode->successor);
    Node *nextOfPrev = getUnmarkedPtr(prevNode)->successor;
    return prevNode->successor.compare_exchange_weak(delNode, nextUnmarked);
}

bool SkipList::helpFlagged(Node *prevNode, Node *delNode) {
    Node *delNodeUnmarked = this->getUnmarkedPtr(delNode);
    delNodeUnmarked->backLink = prevNode;
    if (!this->isMarkedOnPosition(delNodeUnmarked->successor, 1))
        this->tryMark(delNode);
    std::cout << "trying delete : " << delNodeUnmarked->value << std::endl;
    return this->helpMarked(prevNode, delNode);
}

void SkipList::tryMark(Node *node) {
    do {
        Node *nextNode = this->getUnmarkedPtr(node->successor);
        node->successor.compare_exchange_weak(nextNode, this->getMarkedPtr(nextNode, 1));
        if (!this->isMarkedOnPosition(node->successor, 1) && this->isMarkedOnPosition(node->successor, 0))
            this->helpFlagged(node, nextNode->successor);
    } while (!this->isMarkedOnPosition(node->successor, 1));
}


/**
 * returns result word like the tuple of Mikahils (Node*, status, result) where the actual word is a pointer with
 * following pattern: {Node ptr} [0,1]*4
 * so the last 4 bits are status indicators. The meanings of them are (starting from the far rightest):
 * 0: result (0: true; 1: false) indicates if the operation was successful
 * 1: IN; indicates that the node is in List
 * 2: DELETED: indicates tht the node was deleted
 * @param previousNode
 * @param targetNode
 * @return
 */
Node *SkipList::tryFlagNode(Node *previousNode, Node *targetNode) {
    Node *unmarkedPrevious = this->getUnmarkedPtr(previousNode);
    while (true) {
        if (this->isMarkedOnPosition(unmarkedPrevious->successor, 0)) ///vorgaenger ist bereits flagged
            return this->buildResult(previousNode, SKL_IN, false);
        if (this->isMarkedOnPosition(targetNode, 0)) ///zu markierender Node is bereits flagged
            return this->buildResult(previousNode, SKL_IN, false);
        Node *unmarkedTarget = this->getUnmarkedPtr(targetNode);
        if (unmarkedPrevious->successor.compare_exchange_weak(unmarkedTarget, this->getMarkedPtr(unmarkedTarget, 0))) {
            return this->buildResult(previousNode, SKL_IN, true);
        }
        while (this->isMarkedOnPosition(previousNode, 1)) {
            unmarkedPrevious = this->getUnmarkedPtr(unmarkedPrevious->backLink);
            previousNode = unmarkedPrevious->backLink;
        }
        Node *delNode;
        previousNode = this->searchRight(targetNode->value - 1, previousNode, &delNode);
        if (delNode != previousNode)
            return this->buildResult(previousNode, SKL_DELETED, false);
    }
}

Node *SkipList::search(int key, Node **nextNode) {
    Node *curNode;
    curNode = searchToLevel(key, 1, nextNode);
    if (getUnmarkedPtr(curNode)->value == key) {
        return curNode;
    } else
        return nullptr;
}

Node *SkipList::searchToLevel(int key, int level, Node **node) {
    Node *curNode;
    int currentLevel = findStart(level, &curNode);
    while (currentLevel > level) {
        curNode = searchRight(key, curNode, node);
        curNode = getUnmarkedPtr(curNode)->down;
        currentLevel--;
    }
    curNode = searchRight(key, curNode, node);
    return curNode;
}

int SkipList::findStart(int level, Node **resultNode) {
    (*resultNode) = this->headRoot;
    int currentLevel = 1;
    Node *upRightNode = getUnmarkedPtr((*resultNode))->limitUp;
    upRightNode = getUnmarkedPtr(upRightNode->successor);

    while ((upRightNode->value != std::numeric_limits<int>::max()) || (currentLevel < level)) {
        (*resultNode) = (*resultNode)->limitUp;
        upRightNode = (*resultNode)->limitUp;
        upRightNode = upRightNode->successor;
        currentLevel++;
    }
    return currentLevel;

}

/**
 *
 * @param key
 * @param currentNode
 * @param returnNode
 * @return
 */
Node *SkipList::searchRight(int key, Node *currentNode, Node **nextNode) {
    (*nextNode) = this->getUnmarkedPtr(currentNode)->successor;
    while ((*nextNode)->value < key) {
        Node *root = (*nextNode)->towerRoot;
        while (this->isMarkedOnPosition(root, 1)) {
            Node *result = this->tryFlagNode(currentNode, (*nextNode));
            if (this->isIN(result))
                this->helpFlagged(currentNode, (*nextNode));
        }
        if ((*nextNode)->value <= key) {
            currentNode = (*nextNode);
            (*nextNode) = this->getUnmarkedPtr(currentNode)->successor;
        }
    }
    return currentNode;
}

bool SkipList::remove(int key) {
    Node *prevNode;
    Node *delNode;

    prevNode = this->searchToLevel(key, 1, &delNode);
    if (this->getUnmarkedPtr(delNode)->value != key)
        return false;
    if (!removeNode(prevNode, delNode))
        return false;
    searchToLevel(key, 2, &delNode);
    return true;

}

bool SkipList::removeNode(Node *prevNode, Node *delNode) {
    Node *result = this->tryFlagNode(prevNode, delNode);
    prevNode = this->getUnmarkedPtr(result);
    if (this->isIN(result)) {
        this->helpFlagged(prevNode, delNode);
    }
    if (!this->isSuccess(result))
        return false;
    return true;
}

Node *SkipList::buildResult(Node *node, int status, bool success) {
    int successBit = (success ? 1 : 0);
    size_t result = (size_t) node;
    result = result | status | successBit;
    return (Node *) result;
}

bool SkipList::isSuccess(Node *node) {
    return this->isMarkedOnPosition(node, 0);
}

bool SkipList::isIN(Node *node) {
    return this->isMarkedOnPosition(node, 1);
}

bool SkipList::isDeleted(Node *node) {
    return this->isMarkedOnPosition(node, 2);
}

bool SkipList::isDuplicateKey(Node *result) {
    return this->isMarkedOnPosition(result, 3);
}
