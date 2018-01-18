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
    std::atomic<bool *> toTrue;
    std::atomic<bool *> toFalse;
    int *insertsByNonBlock, *insertsByTSX, *deletesByNonBlock, *deletesByTSX;
    long long *absoluteTriesDeleteTSX, *absoluteTriesInsertTSX;

    List();

    ~List();

    bool insert(int key, int threadId);

    bool del(int searchKey, int threadId);
    bool delNonBlocok(int searchKey);

    Node *search(int key, Node **leftNode);


    void print();
    void printStats();
};


#endif //MUTEX_LIST_H
