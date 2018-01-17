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

    List();

    ~List();

    bool insert(int key);

    bool del(int searchKey);

    Node *search(int key, Node **leftNode);

    void print();
};


#endif //MUTEX_LIST_H
