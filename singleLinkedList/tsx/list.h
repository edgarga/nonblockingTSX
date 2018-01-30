//
// Created by edgar on 10.01.18.
//

#ifndef MUTEX_LIST_H
#define MUTEX_LIST_H


#include "node.h"


class List {
public:
    Node *head;
    Node *tail;
//    std::atomic<bool *> toTrue;
//    std::atomic<bool *> toFalse;
    int *insertsByNonBlock, *insertsByTSX, *deletesByNonBlock, *deletesByTSX;
    long long *triesForSuccessfulDeleteTSX, *triesForSuccessfulInsertTSX;
    long long *absoluteDeleteTsxTries, *absoluteInsertTsxTries;
    long long *abortedTsxDeleteTry, *abortedTsxInsertTry;


    int absoluteTries_Insert;
    int absoluteTries_Delete;


    List();

    ~List();

    bool insert(int key, int threadId);

    bool del(int searchKey, int threadId);

    Node *search(int key, Node **leftNode);

    bool isMarkedPtr(size_t node);
    bool isMarkedPtr(Node *node);
    Node* getMarkedPtr(Node *node);
    Node* getUnmarkedPtr(Node *node);

    void print();
    void printStats();
};


#endif //MUTEX_LIST_H
