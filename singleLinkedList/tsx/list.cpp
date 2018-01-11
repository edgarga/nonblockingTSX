//
// Created by edgar on 10.01.18.
//

#define ABSOLUTE_RETRIES 1000
#define RETRIES_OF_ONE_ITERATION 1000

#include <iostream>
#include "list.h"
#include "rTMLock.h"

List::List() {
    this->head = new Node(-2);
    this->tail = new Node(-1);
    this->head->next = this->tail;
}

bool List::insert(int key) {
    Node *newNode = new Node(key);
    Node *rightNode, *leftNode;

    int absoluteTries = 0;
    bool insertedNewNode = false;
    do {
        int triesOfThisIteration = 0;
        rightNode = this->search(key, &leftNode);
        unsigned status;

        if (rightNode->value == key) return false;

        while (triesOfThisIteration < RETRIES_OF_ONE_ITERATION && absoluteTries < ABSOLUTE_RETRIES) {
            //start transaction
            //insert as if no other threads are there

            //check if retry is possible or necessary
            //if not => set triesOfThisIteration to max

            //close transaction if necessary

            //check if node was successful inserted
            {
                LockElision eLock;
                if ((status = eLock.startTransaction()) == _XBEGIN_STARTED && rightNode->value != key) {
                    //TODO: normal insert algorithm goes here
                    newNode->next = rightNode;
                    leftNode->next = newNode;
                }
            }
            if (leftNode->next == newNode && false)
                std::cout << key << " " << leftNode->value << " " << leftNode->next->value << " "
                          << rightNode->value << " " << status << " " << !status << " " << (status & _XABORT_RETRY)
                          << " " << !(status & _XABORT_RETRY) << " " << ((leftNode->next == newNode) ? "==" : "!=")
                          << std::endl;
            if (status & _XABORT_RETRY) {
            } else {
                triesOfThisIteration = RETRIES_OF_ONE_ITERATION;
            }

            if (leftNode->next == newNode && newNode->next == rightNode) { //
                std::cout << status << " " << !status << std::endl;
                return true;
//                insertedNewNode = true;
//                break;

//            if (_xtest()) {
//                _xend();
//            }


                triesOfThisIteration++;
                absoluteTries++;

            }

            if (insertedNewNode) {
//            std::cout << "A: " << triesOfThisIteration << " " << absoluteTries << std::endl;
                return true;
            } else if (absoluteTries >= ABSOLUTE_RETRIES) {
                //TODO: nonblocking insert algorithm goes here
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
 
    do {
        Node *t = this->head;
        Node *tNext = this->head->next;
//        1: Find left and right nodes
        do {

            if (!tNext->markedToDelete) {
                (*leftNode) = t;
                leftNextNode = tNext;
            }
            t = tNext;
            if (t == this->tail) break;
            tNext = t->next;
        } while (t->markedToDelete || (t->value < searchKey));
        rightNode = t;
//        2: Check Nodes are adjacent
        if (leftNextNode == rightNode) {
            if ((rightNode != this->tail) && rightNode->markedToDelete) {
                continue;
            } else {
                return rightNode;
            }
        }

//        if ((*leftNode)->next.compare_exchange_weak(leftNextNode,
//                                                    rightNode)) {
//            if ((rightNode != this->tail) && rightNode->markedToDelete) {
//                //TODO: delete the node properly
//                continue;
//            } else {
//                return rightNode;
//            }
//        }

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
            std::cout << "| " << currentNode->value << " |";

        if (currentNode->next != nullptr)
            std::cout << " -> ";

        currentNode = currentNode->next;
    } while (currentNode->next != nullptr);
    std::cout << "tail";
    std::cout << std::endl;
}

List::~List() {}

bool List::del(int key) {
    return true;
}


