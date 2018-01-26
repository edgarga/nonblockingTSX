//
// Created by edgar on 10.01.18.
//

#ifndef MUTEX_NODE_H
#define MUTEX_NODE_H


#include <atomic>
#include "lockElision.h"

class Node {
public:
    int key;
//    std::atomic<bool *> marked;
//    std::atomic<bool *> deleted;
    std::atomic<Node *> next;

//    Node();

//    Node(int key, bool *ptr);
    Node(int key);

    ~Node();

};


#endif //MUTEX_NODE_H
