//
// Created by edgar on 10.01.18.
//

#ifndef MUTEX_NODE_H
#define MUTEX_NODE_H


#include "lockElision.h"

class Node {
public:
    int value;
    Node *next;
    bool markedToDelete;

    Node();
    Node(int val);
    ~Node();

};


#endif //MUTEX_NODE_H
