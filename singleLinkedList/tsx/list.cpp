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
    this->tail->prior = this->head;
}

bool List::isMarkedPtr(size_t node) {
    return (node & 1) != 0;
}

bool List::isMarkedPtr(Node *node) {
    return isMarkedPtr((size_t) node);
}

/**
 * Marks the pointer to the node object by setting the smallest bit
 * @param node a pointer to a Node object
 * @return returns node | 1 if node is not marked, node otherwise
 */
Node *List::getMarkedPtr(Node *node) {
    size_t ptr = (size_t) node;
    ptr = ptr | 1;
    return (Node *) ptr;

}

/**
 * Unmakrs the pointer ot he Node object be unsetting the smallest bit
 * @param node a pointer to a Node object
 * @return returns the pointer with the least bit not set
 */
Node *List::getUnmarkedPtr(Node *node) {
    size_t ptr = (size_t) node;
    ptr = ptr & (std::numeric_limits<std::size_t>::max() - 1);
    return (Node *) ptr;
}

bool List::areMarkedInbetween(Node *start, Node *end) {
//    std::cout << "z" << std::endl;
    Node *curr = start->next;
    while (this->isMarkedPtr(curr->next) && curr != end) {
        if (curr == this->tail) return false;
        curr = this->getUnmarkedPtr(curr->next);
    }
    if (curr == end)
        return true;
    else
        return false;
}

bool List::insert(int key, int threadId) {
    Node *newNode = new Node(key);
    Node *rightNode, *leftNode;

    int absoluteTries = 0;
    do {
        unsigned status;
        if (absoluteTries < this->absoluteTries_Insert) {
            if (this->useTsxSearch)
                rightNode = tsxSearch(key, &leftNode);
            else
                rightNode = search(key, &leftNode);
            if (rightNode->key == key) return false;


            while (absoluteTries < this->absoluteTries_Insert) {
                absoluteTries++;
                unsigned long long int start = __rdtsc();
                LockElision eLock;
                if ((status = eLock.startTransaction()) == _XBEGIN_STARTED) { /// check if transaction was started
                    if ((leftNode->next != rightNode) ||
                        this->isMarkedPtr(leftNode) || this->isMarkedPtr(rightNode)) {

                        break;
                    }

                    newNode->prior = leftNode;
                    newNode->next = rightNode; /// insert new node
                    leftNode->next = newNode;
                    rightNode->prior = newNode;
                }
                if ((eLock.endTransaction() &&
                     (status == _XBEGIN_STARTED || status == 0))) { /// return if insert was successful
                    if (threadId >= 0) {
                        this->triesForSuccessfulInsertTSX[threadId] += absoluteTries;
                        this->absoluteInsertTsxTries[threadId] += absoluteTries;
                        this->insertsByTSX[threadId]++;
                        unsigned long long int ticks = __rdtsc() - start;
                        this->tsxInsertTimeCount[threadId]++;
                        this->insertTicks[threadId] += ticks;
                    }
                    return true;
                }


                if (status & _XABORT_RETRY) { /// do nothing if can be retried
                    if (threadId >= 0) {
                        this->abortedTsxInsertTry[threadId]++;
                        unsigned long long int ticks = __rdtsc() - start;
                        this->tsxInsertTimeCount[threadId]++;
                        this->insertTicks[threadId] += ticks;
                    }
                } else {
                    break; /// new search will be initiated
                }
            }


        }

        if (absoluteTries >= this->absoluteTries_Insert) {
            rightNode = this->search(key, &leftNode);
            if ((rightNode != this->tail) && (rightNode->key == key)) {
                return false;
            }

            newNode->next = rightNode;
            newNode->prior = leftNode;
            if (leftNode->next.compare_exchange_weak(rightNode, newNode)) {
                if (threadId >= 0) {
                    this->absoluteInsertTsxTries[threadId] += absoluteTries;
                    this->insertsByNonBlock[threadId]++;
                }
                return true;
            }
        }


    } while (true);


}

Node *List::tsxSearch(int key, Node **leftNode) {

    Node *t_frontSearch = this->head;
    Node *tNext = this->head->next;
    Node *t_backSearch = this->tail;
    Node *tPrior = t_backSearch->prior;
//        1: Find left and right nodes
    bool searchBack = true;
    do {
        if (!isMarkedPtr(tNext)) {
            (*leftNode) = t_frontSearch;
        }
        t_frontSearch = getUnmarkedPtr(tNext);
        if (t_frontSearch == this->tail)
            break;
        tNext = t_frontSearch->next;

        if (searchBack) {
//            std::cout << "a" << std::endl;
            if (tPrior == this->head || t_backSearch->key < key) {
                searchBack = false;
            } else {
                if (tPrior->key < key && !this->isMarkedPtr(tPrior) && !this->isMarkedPtr(t_backSearch)) {
                    (*leftNode) = tPrior;
                    t_frontSearch = tPrior;
                    tNext = tPrior->next;
//                    tNext = t_backSearch;
                    searchBack = false;
                    goto edgar;
//                    return t_backSearch;
                }
                if (!this->isMarkedPtr(tPrior->next))
                    t_backSearch = tPrior;
                tPrior = tPrior->prior;
            }
        }

        edgar:;
    } while (isMarkedPtr(tNext) ||
             (t_frontSearch->key < key));
    return t_frontSearch;

}

/**
 * searches the the Node with the specified key and deletes marked Nodes
 * @param searchKey
 * @param leftNode
 * @return if key exists: the Node with the specified key, tail otherwise
 */
Node *List::search(int searchKey, Node **leftNode) {
    Node *leftNextNode = nullptr, *rightNode = nullptr;
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
//                delete rightNode;
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

List::~List() {
    std::cout << "deleting list" << std::endl;
//    delete this->tail;
//    delete this->head;
//    delete this->useTsxSearch;
//    delete
}

bool List::del(int searchKey, int threadId) {

    Node *rightNode, *rightNextNode, *leftNode;


    int absoluteTries = 0;

    unsigned status;

    do {
        if (absoluteTries < this->absoluteTries_Delete) {
            if (this->useTsxSearch)
                rightNode = tsxSearch(searchKey, &leftNode);
            else
                rightNode = search(searchKey, &leftNode);
            if (rightNode == this->tail || rightNode->key != searchKey) return false;
            rightNextNode = rightNode->next;
            while (absoluteTries < this->absoluteTries_Delete) {
                absoluteTries++;
                LockElision eLock;
                if ((status = eLock.startTransaction()) == _XBEGIN_STARTED) { /// check if transaction was started
                    if ((leftNode->next != rightNode) || rightNode->next != rightNextNode ||
                        this->isMarkedPtr(rightNextNode) || this->isMarkedPtr(leftNode)) {

                        break;
                    }
                    rightNode->next = getMarkedPtr(rightNode->next);
//                rightNextNode = rightNode->next;
                    leftNode->next = rightNextNode;
                    rightNextNode->prior = leftNode;
                }
                if ((eLock.endTransaction() && status == _XBEGIN_STARTED)) {
                    if (threadId >= 0) {
                        this->deletesByTSX[threadId]++;
                        this->triesForSuccessfulDeleteTSX[threadId] += absoluteTries;
                        this->absoluteDeleteTsxTries[threadId] += absoluteTries;
                    }
//                    delete rightNode;
                    return true;
                }

                if (status & _XABORT_RETRY) { /// do nothing if can be retried
                    if (threadId) { this->abortedTsxDeleteTry[threadId]++; }
                } else {
                    break; /// new search will be initiated
                }


            }
        }

        if (absoluteTries >= this->absoluteTries_Delete) {
            rightNode = search(searchKey, &leftNode);
            if ((rightNode == this->tail) || (rightNode->key != searchKey)) {
                if (threadId >= 0) { this->absoluteDeleteTsxTries[threadId] += absoluteTries; }
                return false;
            }
            rightNextNode = rightNode->next;

            if (!isMarkedPtr(rightNextNode)) {
                if (rightNode->next.compare_exchange_weak(rightNextNode, getMarkedPtr(rightNextNode))) {
                    rightNextNode->prior = leftNode;
                    break;
                }
            }
        }
    } while (true);
    if (!(leftNode->next.compare_exchange_weak(rightNode, rightNextNode))) {
        rightNode = this->search(searchKey, &leftNode);
    }
//    else
//        delete rightNode;

    if (threadId >= 0) {
        this->absoluteDeleteTsxTries[threadId] += absoluteTries;
        this->deletesByNonBlock[threadId]++;
    }
    return true;
}

bool List::isIncreasing() {
    Node *current = this->head;
    int lastValue = current->key;
    while (current != this->tail) {
        current = current->next;

        if (lastValue >= current->key)
            return false;
    }

    return true;
}


void List::printStats() {
//    int absoluteSuccessfulInserts = 0, absoluteSuccessfulDeletes = 0;
//    long long absoluteTSXInsertTries = 0, absoluteTSXDeleteTries = 0;
}


