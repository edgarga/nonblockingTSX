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
    lowerHead->level = 1;
    Node *lowerTail = new Node(true, false);
    this->headRoot = lowerHead;
    this->tailRoot = lowerTail;
    lowerHead->successor = lowerTail;

    lowerHead->towerRoot = lowerHead;
    lowerTail->towerRoot = lowerTail;

    for (int i = 0; i < this->maxLevel; i++) {
        Node *newHead = new Node(true, true);
        newHead->level = i + 2;
        Node *newTail = new Node(true, false);
        newHead->successor = newTail;

        newHead->down = lowerHead;
        newTail->down = lowerTail;

        lowerHead->limitUp = newHead;
        lowerTail->limitUp = newTail;

        lowerHead = newHead;
        lowerTail = newTail;

        lowerHead->towerRoot = lowerHead;
        lowerTail->towerRoot = lowerTail;
    }

    lowerHead->limitUp = lowerHead;
    lowerTail->limitUp = lowerTail;

}


bool SkipList::insert(int key) {
    Node *prevNode, *nextNode;
    prevNode = searchToLevel(key, 1, &nextNode, -21);

    if (prevNode->value == key)
        return false;

    Node *newRootNode = new Node(key);
    newRootNode->towerRoot = newRootNode;
    Node *newNode = newRootNode;
    int towerHeight = ((rand() % 2) * (rand() % (this->maxLevel - 2))) + 1;

    int currentLevel = 1;
    while (true) {
        Node *result;
        if (newNode->towerRoot == nullptr)
            std::cout << "newN.root = null " << currentLevel << std::endl;
        prevNode = insertNode(newNode, prevNode, nextNode, &result);
        if (isDuplicateKey(result) && currentLevel == 1)
            return false;
        if (isMarkedOnPosition(newRootNode->successor, 1)) {
            if ((getNode(result) == newNode) && (newNode != newRootNode))
                removeNode(prevNode, newNode, -22);
            return true;
        }

        currentLevel++;

        if (currentLevel == towerHeight + 1)
            return true;

        Node *lastNode = newNode;
        newNode = new Node(key);
        newNode->down = lastNode;
        newNode->towerRoot = newRootNode;
        prevNode = searchToLevel(key, currentLevel, &nextNode, -23);
    }


}

Node *SkipList::insertNode(Node *newNode, Node *prevNode, Node *nextNode, Node **result) {
    if (getNode(prevNode)->value == newNode->value) {
        size_t resultSequence = SKL_DUPLICATE_KEY;
        (*result) = (Node *) resultSequence;
        return prevNode;
    }

    int i = 0;
    while (true) {
        Node *nextOfPrev = getNode(prevNode)->successor;
        if (isMarkedOnPosition(nextOfPrev, 0))
            helpFlagged(prevNode, getNode(nextOfPrev), -31);
        else {
            newNode->successor = getNode(nextNode);
            Node *nextUnmarked = getNode(nextNode);
            bool nextIsMarked = isMarked(nextUnmarked);
            int val = nextUnmarked->value;

            if (getNode(prevNode)->successor.compare_exchange_strong(nextUnmarked, newNode)) {
                (*result) = newNode;
                return prevNode;
            } else {
                Node *prevSucc = (getNode(prevNode)->successor);
                Node *un = getNode(prevSucc);
                size_t pSPtr = (size_t) prevSucc;

                if (isMarkedOnPosition(getNode(prevNode)->successor, 0))
                    helpFlagged(prevNode, un, -32);
                while (isMarkedOnPosition(getNode(prevNode)->successor, 1)) {
                    prevNode = getNode(prevNode)->backLink;
                }
            }

        }
        prevNode = searchRight(newNode->value, prevNode, &nextNode, -24);
        if (getNode(prevNode)->value == newNode->value) {
            size_t resultSequence = SKL_DUPLICATE_KEY;
            (*result) = (Node *) resultSequence;
            return prevNode;
        }
        i++;
    }


}

bool SkipList::remove(int key, int threadId) {
    Node *prevNode;
    Node *delNode;

    prevNode = searchToLevel(key - 1, 1, &delNode, threadId);
    if (getNode(delNode)->value != key)
        return false;
    if (!removeNode(prevNode, delNode, threadId)) {

        return false;
    }
    searchToLevel(key, 2, &delNode, threadId);
    return true;

}

bool SkipList::removeNode(Node *prevNode, Node *delNode, int threadId) {
    Node *result = this->tryFlagNode(prevNode, delNode);
    Node *resultNode = getNode(result);
    prevNode = this->getNode(result);
    if (isIN(result)) {
        helpFlagged(prevNode, delNode, threadId);
    }
    if (!isSuccess(result))
        return false;
    return true;
}

Node *SkipList::search(int key, Node **nextNode) {
    Node *curNode;
    curNode = searchToLevel(key, 1, nextNode, -66);
    if (getNode(curNode)->value == key) {
        return curNode;
    } else
        return nullptr;
}

Node *SkipList::searchToLevel(int key, int level, Node **nextReturnNode, int threadId) {
    Node *next;
    Node *curNode;
    int currentLevel = findStart(level, &curNode);
    while (currentLevel > level) {
        curNode = searchRight(key, curNode, &next, threadId);
        curNode = getNode(curNode)->down;
        currentLevel--;
    }
    (*nextReturnNode) = next;
    curNode = searchRight(key, curNode, nextReturnNode, threadId);
    return curNode;
}


int SkipList::findStart(int level, Node **resultNode) {
    (*resultNode) = this->headRoot;
    int currentLevel = 1;
    Node *upRightNode = getNode(*resultNode)->limitUp;
    upRightNode = getNode(upRightNode->successor);

    while ((upRightNode->value != std::numeric_limits<int>::max()) || (currentLevel < level)) {
        (*resultNode) = (*resultNode)->limitUp;
        upRightNode = (*resultNode)->limitUp;
        upRightNode = getNode(upRightNode->successor);
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
Node *SkipList::searchRight(int key, Node *currentNode, Node **nextNode, int threadId) {
    (*nextNode) = getNode(currentNode)->successor;
    (*nextNode) = getNode(*nextNode);
    int i = 0;
    while ((*nextNode)->value <= key) {
        Node *root = (*nextNode)->towerRoot;
        int j = 0;
        while (this->isMarkedOnPosition(root->successor, 1)) {
            Node *result = this->tryFlagNode(currentNode, (*nextNode));
            currentNode = getNode(result);
            if (isIN(result))
                helpFlagged(currentNode, (*nextNode), threadId);

            (*nextNode) = getNode(getNode(currentNode)->successor);

            root = (*nextNode)->towerRoot;
            j++;
        }
        if ((*nextNode)->value <= key) {
            currentNode = (*nextNode);
            (*nextNode) = getNode(getNode(currentNode)->successor);
        }
        i++;
    }
    return currentNode;
}

bool SkipList::helpMarked(Node *prevNode, Node *delNode, int threadId) {
    Node *nextUnmarked = getNode(getNode(delNode)->successor);
    Node *flaggedDelNode = getMarkedPtr(delNode, 0);
    bool is = false;
    if (isMarkedOnPosition(flaggedDelNode, 0) && flaggedDelNode != delNode)
        is = true;
    bool wasSuccessful = getNode(prevNode)->successor.compare_exchange_strong(flaggedDelNode, nextUnmarked); //NULL
//    if(getNode(prevNode)->value != std::numeric_limits<int>::max() && nextUnmarked == nullptr)
//        std::cout << " new node succ null " << std::endl;


    return wasSuccessful;

}

bool SkipList::helpFlagged(Node *prevNode, Node *delNode, int threadId) {
    getNode(delNode)->backLink = getNode(prevNode);
    if (!isMarkedOnPosition(getNode(delNode)->successor, 1)) {

        tryMark(delNode, threadId);
    }
    return helpMarked(prevNode, delNode, threadId);
}

void SkipList::tryMark(Node *delNode, int threadId) {
    int i = 0;
    if (getNode(delNode)->isLimitingNode)
        std::cout << "del node is Limit" << std::endl;
    do {
        Node *nextOfDel = getNode(delNode)->successor;
        Node *nextNodeUnmarked = getNode(nextOfDel);
        if (!getNode(delNode)->successor.compare_exchange_strong(nextNodeUnmarked, getMarkedPtr(nextNodeUnmarked, 1))) {
            if (!isMarkedOnPosition(nextOfDel, 1) &&
                isMarkedOnPosition(nextOfDel, 0)) {
                helpFlagged(getNode(delNode), nextNodeUnmarked, threadId);
            }
        }
        i++;
    } while (!isMarkedOnPosition(getNode(delNode)->successor, 1));
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
//    Node *unmarkedPrevious = this->getNode(previousNode);
    if (getNode(targetNode)->isLimitingNode && !getNode(targetNode)->isHeadNode)
        std::cout << "trying flagging limit node" << std::endl;
    while (true) {
        Node *prevUnmarked = getNode(previousNode);
        Node *sucOfPref = prevUnmarked->successor;
        if (getNode(sucOfPref) == getNode(targetNode) &&
            isMarkedOnPosition(getNode(sucOfPref)->successor, 0)) ///vorgaenger ist bereits flagged
            return buildResult(previousNode, SKL_IN, false);
        Node *unmarkedTarget = getNode(targetNode);
        if (getNode(previousNode)->successor.compare_exchange_strong(unmarkedTarget,
                                                                     this->getMarkedPtr(unmarkedTarget, 0))) {
            return buildResult(previousNode, SKL_IN, true);
        }

        if (getNode(getNode(previousNode)->successor) == getNode(targetNode) &&
            isMarkedOnPosition(getNode(previousNode)->successor, 0)) ///zu markierender Node is bereits flagged
            return buildResult(previousNode, SKL_IN, false);

        while (isMarkedOnPosition(getNode(previousNode)->successor, 1)) {
            previousNode = getNode(previousNode)->backLink;
        }
        Node *delNode;
        previousNode = searchRight(getNode(targetNode)->value - 1, getNode(previousNode), &delNode, -5);
        if (getNode(delNode) != getNode(previousNode))
            return buildResult(previousNode, SKL_DELETED, false);
    }
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

SkipList::~SkipList() {

}

bool SkipList::isMarked(size_t ptr) {
    return (ptr & 15) != 0;
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

Node *SkipList::getNode(Node *node) {
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

void SkipList::print() {
    int count = 0;
    Node *cur = this->headRoot;
    while (cur != nullptr) {

        if (isMarkedOnPosition(cur->successor, 1))
            std::cout << " |";

        std::cout << cur->value;

        if (isMarkedOnPosition(cur->successor, 0))
            std::cout << "| ";
        std::cout << " -> ";

        cur = this->getNode(cur->successor);
        count++;
    }

    std::cout << "NULL" << std::endl;
}
