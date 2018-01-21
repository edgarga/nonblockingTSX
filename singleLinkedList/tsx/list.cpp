//
// Created by edgar on 10.01.18.
//

#define INSERT_ABSOLUTE_RETRIES 50
#define INSERT_RETRIES_OF_ONE_ITERATION 2

#define DELETE_ABSOLUTE_RETRIES 50
#define DELETE_RETRIES_OF_ONE_ITERATION 2

#include <iostream>
#include "list.h"

List::List() {
    this->toTrue = new bool(true);
    this->toFalse = new bool(false);
    this->head = new Node(-3, this->toFalse);
    this->tail = new Node(-1, this->toFalse);
    this->head->next = this->tail;
}

bool List::insert(int key, int threadId) {
    Node *newNode = new Node(key, this->toFalse);
    Node *rightNode, *leftNode;

    int absoluteTries = 0;
    do {
        int triesOfThisIteration = 0;
        rightNode = this->search(key, &leftNode);
        unsigned status;

        if (rightNode->key == key) return false;

        while (triesOfThisIteration < INSERT_RETRIES_OF_ONE_ITERATION && absoluteTries < INSERT_ABSOLUTE_RETRIES) {
            triesOfThisIteration++;
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
                triesOfThisIteration = INSERT_RETRIES_OF_ONE_ITERATION; /// new search will be initiated
            }


        }

        if (absoluteTries >= INSERT_ABSOLUTE_RETRIES) {
//            std::cout << "inserting via nonBlocking" << std::endl;
            rightNode = this->search(key, &leftNode);

            if ((rightNode != this->tail) && (rightNode->key == key)) {
                return false;
            }
            newNode->next = rightNode;

            if (leftNode->next.compare_exchange_weak(rightNode, newNode)) {
                if (threadId >= 0)
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

bool List::del(int searchKey, int threadId) {

    Node *rightNode, *rightNextNode, *leftNode;

    int triesOfThisIteration = 0, absoluteTries = 0;

    unsigned status;

    do {
        triesOfThisIteration = 0;
        rightNode = search(searchKey, &leftNode);
        if (rightNode == this->tail || rightNode->key != searchKey) return false;
//        std::cout << "s: " << searchKey << " g: " << rightNode->key << " at: " << absoluteTries << std::endl;
        rightNextNode = rightNode->next;
        while (triesOfThisIteration < DELETE_RETRIES_OF_ONE_ITERATION && absoluteTries < DELETE_ABSOLUTE_RETRIES) {
            triesOfThisIteration++;
            absoluteTries++;
//            {/// try with TSX
            bool *toT = this->toTrue;
            LockElision eLock;
            if ((status = eLock.startTransaction()) == _XBEGIN_STARTED) { /// check if transaction was started
                if (leftNode->next != rightNode || rightNode->next !=
                                                   rightNextNode || *rightNode->marked == true) { /// check if node were not change since search and start of transaction

                    break;
                }
                rightNode->marked = toT;
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
                triesOfThisIteration = DELETE_RETRIES_OF_ONE_ITERATION; /// new search will be initiated
            }


        }

        if (absoluteTries >= DELETE_ABSOLUTE_RETRIES) {

//            std::cout << "deleting via nonBlock" << std::endl;

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

    if (threadId >= 0)
        this->deletesByNonBlock[threadId]++;
    return true;
}

bool List::delNonBlocok(int searchKey) {
    return false;
}

void List::printStats() {
//    int absoluteSuccessfulInserts = 0, absoluteSuccessfulDeletes = 0;
//    long long absoluteTSXInsertTries = 0, absoluteTSXDeleteTries = 0;
}


