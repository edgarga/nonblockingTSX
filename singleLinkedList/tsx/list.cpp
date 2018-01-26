//
// Created by edgar on 10.01.18.
//

#include <iostream>
#include <limits>
#include "list.h"

List::List() {

    this->absoluteTries_Insert = 50;
    this->absoluteTries_Delete = 50;

    this->head = new Node(-3);
    this->tail = new Node(std::numeric_limits<int>::max());
    this->head->next = this->tail;
}
bool List::isMarkedPtr(size_t node) {
    return (node & 1) != 0;
}

bool List::isMarkedPtr(Node *node) {
    return isMarkedPtr((size_t) node);
}

Node *List::getMarkedPtr(Node *node) {
    size_t ptr = (size_t) node;
    if (isMarkedPtr(ptr)) {
        return (Node *) (ptr);
    } else {
        ptr = ptr | 1;
        return (Node *) ptr;
    }
}

Node *List::getUnmarkedPtr(Node *node) {
    size_t ptr = (size_t) node;
    if (!isMarkedPtr(ptr)) {
        return (Node *) ptr;
    } else {
        ptr = ptr & (std::numeric_limits<std::size_t>::max() - 1);
        return (Node *) ptr;
    }
}

bool List::insert(int key, int threadId) {
    Node *newNode = new Node(key);
    Node *rightNode, *leftNode;

    int absoluteTries = 0;
    do {

        rightNode = this->search(key, &leftNode);
        unsigned status;

        if (rightNode->key == key) return false;

        while (absoluteTries < this->absoluteTries_Insert) {

            absoluteTries++;
//            { /// start critical Section
            LockElision eLock;
            if ((status = eLock.startTransaction()) == _XBEGIN_STARTED) { /// check if transaction was started
                if (leftNode->next !=
                    rightNode) /// check if node were not change since search and start of transaction
                    break;

                newNode->next = rightNode; /// insert new node
                leftNode->next = newNode;
            }
//            } /// end critical section

            if ((eLock.endTransaction() && status == _XBEGIN_STARTED)
//                && (leftNode->next == newNode && newNode->next == rightNode)
                    ) { /// return if insert was successful
                if (threadId >= 0) {
                    this->absoluteTriesInsertTSX[threadId] += absoluteTries;
                    this->insertsByTSX[threadId]++;
                }
                return true;
            }
            if (status & _XABORT_RETRY) { /// do nothing if can be retried
            } else {
                break; /// new search will be initiated
            }


        }

        if (absoluteTries >= this->absoluteTries_Insert) {
            if ((rightNode != this->tail) && (rightNode->key == key)) {
                return false;
            }
            newNode->next = rightNode;

            if (leftNode->next.compare_exchange_weak(rightNode, newNode)) {
                this->insertsByNonBlock[threadId]++;
                return true;
            }
        }


    } while (true);


}


/**
 * searches the the Node with the specified key and deletes marked Nodes
 * @param searchKey
 * @param leftNode
 * @return if key exists: the Node with the specified key, tail otherwise
 */
Node *List::search(int searchKey, Node **leftNode) {
    Node *leftNextNode, *rightNode;
    int i = 0;
    do {
        Node *t = this->head;
        Node *tNext = this->head->next;
//        1: Find left and right nodes
        do {
            i++;
            if (!isMarkedPtr(tNext)) {
                (*leftNode) = t;
                leftNextNode = tNext;
            }
            t = getUnmarkedPtr(tNext);
            if (t == this->tail)
                break;
            tNext = t->next;
        } while (isMarkedPtr(tNext) ||
                 (t->key < searchKey));
        rightNode = t;

//        2: Check Nodes are adjacent
        if (leftNextNode == rightNode) {
            if ((rightNode != this->tail) && isMarkedPtr(rightNode->next)) {
                continue;
            } else {
                return rightNode;
            }
        }
        if ((*leftNode)->next.compare_exchange_weak(leftNextNode,
                                                    rightNode)) {
            if ((rightNode != this->tail) && isMarkedPtr(rightNode)) {
                //TODO: delete the node properly
                continue;
            } else {
                return rightNode;
            }
        }

    } while (true);
}

/**
 * prints the list and all its Nodes
 */
void List::print() {
    Node *currentNode = this->head;

    do {
        if (currentNode == this->head)
            std::cout << "head";
        else
            std::cout << "| " << currentNode->key << (this->isMarkedPtr(currentNode->next) ? "X" : "") << " |";

        if (currentNode->next != nullptr)
            std::cout << " -> ";

        currentNode = this->getUnmarkedPtr(currentNode->next);
    } while (currentNode->next != nullptr);
    std::cout << "tail";
    std::cout << std::endl;
}

List::~List() {}

bool List::del(int searchKey, int threadId) {

    Node *rightNode, *rightNextNode, *leftNode;

    int  absoluteTries = 0;

    unsigned status;

    do {
        rightNode = search(searchKey, &leftNode);
        if (rightNode == this->tail || rightNode->key != searchKey) return false;
//        std::cout << "s: " << searchKey << " g: " << rightNode->key << " at: " << absoluteTries << std::endl;
        rightNextNode = rightNode->next;
        while (absoluteTries < this->absoluteTries_Delete) {
            absoluteTries++;
//            {/// try with TSX
            LockElision eLock;
            if ((status = eLock.startTransaction()) == _XBEGIN_STARTED) { /// check if transaction was started
                if (leftNode->next != rightNode || rightNode->next !=
                                                   rightNextNode || this->isMarkedPtr(
                        rightNode->next)) { /// check if node were not change since search and start of transaction

                    break;
                }
                rightNode->next = getMarkedPtr(rightNode->next);
                rightNextNode = rightNode->next;
                leftNode->next = rightNextNode;
            }
//            }
            if (
                    (eLock.endTransaction() && status == _XBEGIN_STARTED)
//                &&
//                            (leftNode->next == rightNextNode)
                    ) {
//            if (status == 0xffffffff || leftNode->next == rightNextNode) { /// return if insert was successful
                if (threadId >= 0) {
                    this->deletesByTSX[threadId]++;
                    this->absoluteTriesDeleteTSX[threadId] += absoluteTries;
                }
                return true;
            }

            if (status & _XABORT_RETRY) { /// do nothing if can be retried
            } else {
                break; /// new search will be initiated
            }


        }

        if (absoluteTries >= this->absoluteTries_Delete) {
            if ((rightNode == this->tail) || (rightNode->key != searchKey)) {
                return false;
            }
            rightNextNode = rightNode->next;

            if (!isMarkedPtr(rightNextNode)) {
                if (rightNode->next.compare_exchange_weak(rightNextNode, getMarkedPtr(rightNextNode))) {
                    break;
                }
            }
        }
    } while (true);
    if (!(leftNode->next.compare_exchange_weak(rightNode, rightNextNode))) {
        rightNode = this->search(searchKey, &leftNode);
    }

    if (threadId >= 0)
        this->deletesByNonBlock[threadId]++;
    return true;
}



void List::printStats() {
//    int absoluteSuccessfulInserts = 0, absoluteSuccessfulDeletes = 0;
//    long long absoluteTSXInsertTries = 0, absoluteTSXDeleteTries = 0;
}


