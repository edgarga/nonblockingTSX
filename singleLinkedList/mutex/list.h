//
// Created by edgar on 07.01.18.
//

#include <mutex>

#include "node.h"

#ifndef MUTEX_LIST_H
#define MUTEX_LIST_H
class List{
public:
    Node *head, *tail;
    std::mutex listMtx;

    List();
    ~List();

    bool insert(int val);
    Node* search(int val, Node **leftNode);
    Node* predecessorOf(int val);
    bool del(int val);

    void print();
};
#endif //MUTEX_LIST_H
