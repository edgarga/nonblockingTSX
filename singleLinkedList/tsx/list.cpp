//
// Created by edgar on 10.01.18.
//

#define ABSOLUTE_RETRIES 100
#define RETRIES_OF_ONE_ITERATION 100

#include <iostream>
#include "list.h"

List::List() {
    this->toTrue = new bool(true);
    this->toFalse = new bool(false);
    this->head = new Node(-3, this->toFalse);
    this->tail = new Node(-1, this->toFalse);
    this->head->next = this->tail;
}

bool List::insert(int key) {
    Node *newNode = new Node(key, this->toFalse);
    Node *rightNode, *leftNode;

    int absoluteTries = 0;
    bool insertedNewNode = false;
    do {
        int triesOfThisIteration = 0;
        rightNode = this->search(key, &leftNode);
        unsigned status;

        if (rightNode->key == key) return false;

        while (triesOfThisIteration < RETRIES_OF_ONE_ITERATION && absoluteTries < ABSOLUTE_RETRIES) {
            triesOfThisIteration++;
            absoluteTries++;
//            { /// start critical Section
            LockElision eLock;
            if ((status = eLock.startTransaction()) == _XBEGIN_STARTED) { /// check if transaction was started
                if (leftNode->next !=
                    rightNode) /// check if node were not change since search and start of transaction
                    continue;

                newNode->next = rightNode; /// insert new node
                leftNode->next = newNode;
            }
//            } /// end critical section

            if (eLock.endTransaction()  &&
                (leftNode->next == newNode && newNode->next)) { /// return if insert was successful
                return true;
            }
            if (status & _XABORT_RETRY) { /// do nothing if can be retried
            } else {
                triesOfThisIteration = RETRIES_OF_ONE_ITERATION; /// new search will be initiated
            }


        }

        if (!insertedNewNode && absoluteTries >= ABSOLUTE_RETRIES) {
//            std::cout << "inserting via nonBlocking" << std::endl;
            rightNode = this->search(key, &leftNode);

            if ((rightNode != this->tail) && (rightNode->key == key)) {
                return false;
            }
            newNode->next = rightNode;

            if (leftNode->next.compare_exchange_weak(rightNode, newNode)) {
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

    do {
//        std::cout << "doSearch" << std::endl;
        Node *t = this->head;
        Node *tNext = this->head->next;
//        1: Find left and right nodes
        do {
            if (*t->marked == false) {
                (*leftNode) = t;
                leftNextNode = tNext;
            }
            t = tNext;
            if (t == this->tail) break;
            tNext = t->next;
        } while (*t->marked == true || (t->key < searchKey));
//        std::cout << t->key << std::endl;
        rightNode = t;
//        2: Check Nodes are adjacent
        if (leftNextNode == rightNode) {
            if ((rightNode != this->tail) && *rightNode->marked == true) {
                continue;
            } else {
                return rightNode;
            }
        }
        if ((*leftNode)->next.compare_exchange_weak(leftNextNode,
                                                    rightNode)) {
            if ((rightNode != this->tail) && *rightNode->marked == true) {
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
            std::cout << "| " << currentNode->key << " |";

        if (currentNode->next != nullptr)
            std::cout << " -> ";

        currentNode = currentNode->next;
    } while (currentNode->next != nullptr);
    std::cout << "tail";
    std::cout << std::endl;
}

List::~List() {}

bool List::del(int searchKey) {

    Node *rightNode, *rightNextNode, *leftNode;

    int triesOfThisIteration = 0, absoluteTries = 0;

    unsigned status;

    do {
        triesOfThisIteration = 0;
        rightNode = search(searchKey, &leftNode);
        if (rightNode == this->tail || rightNode->key != searchKey) return false;
//        std::cout << "s: " << searchKey << " g: " << rightNode->key << " at: " << absoluteTries << std::endl;
        rightNextNode = rightNode->next;
        while (triesOfThisIteration < RETRIES_OF_ONE_ITERATION && absoluteTries < ABSOLUTE_RETRIES) {
            triesOfThisIteration++;
            absoluteTries++;
//            {/// try with TSX
            LockElision eLock;
            if ((status = eLock.startTransaction()) == _XBEGIN_STARTED) { /// check if transaction was started
                if (leftNode->next != rightNode || rightNode->next !=
                                                   rightNextNode) { /// check if node were not change since search and start of transaction

                    break;
                }

//                    rightNode->marked = *this->toTrue;
                rightNextNode = rightNode->next;
                leftNode->next = rightNextNode;
            }
//            }
            if (eLock.endTransaction() && leftNode->next == rightNextNode) {
//            if (status == 0xffffffff || leftNode->next == rightNextNode) { /// return if insert was successful
                return true;
            }
            if (status & _XABORT_RETRY) { /// do nothing if can be retried
            } else {
                triesOfThisIteration = RETRIES_OF_ONE_ITERATION; /// new search will be initiated
            }


        }

        if (absoluteTries >= ABSOLUTE_RETRIES) {

            std::cout << "deleting via nonBlock" << std::endl;

            if ((rightNode == this->tail) || (rightNode->key != searchKey)) {
                return false;
            }
            rightNextNode = rightNode->next;

            if (*rightNode->marked != true) {
                bool *one = this->toFalse;
                bool *two = this->toTrue;
                if (rightNode->marked.compare_exchange_weak(one, two)) {
                    break;
                }
            }
        }
    } while (true);
    if (!(leftNode->next.compare_exchange_weak(rightNode, rightNextNode))) {
        rightNode = this->search(searchKey, &leftNode);
    }

    return true;
}


