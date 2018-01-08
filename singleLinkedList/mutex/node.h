//
// Created by edgar on 07.01.18.
//
#include <mutex>
#ifndef MUTEX_NODE_H
#define MUTEX_NODE_H
class Node{
public:
    int value;
    std::mutex mtx;
    bool deleted;
    Node *next;

    Node();
    Node(int value);
    ~Node();

    bool setNextProtected(Node* newNext);
};
#endif //MUTEX_NODE_H
