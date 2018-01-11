//
// Created by edgar on 10.01.18.
//

#ifndef MUTEX_LIST_H
#define MUTEX_LIST_H


#include "node.h"

class List {
public:
    Node* head;
    Node* tail;

    List();
    ~List();

    Node* search(int key, Node **leftNode);
    bool insert(int key);
    bool del(int key);
    void print();

};


#endif //MUTEX_LIST_H
