//
// Created by edgar on 12.02.18.
//

#ifndef TSX_NODE_H
#define TSX_NODE_H


#include <atomic>

class Node {
public:
    int value;
    std::atomic<Node *> backLink;
    std::atomic<Node *> successor;
    std::atomic<Node *> down;
    std::atomic<Node *> towerRoot;

    ///Limiting node Properties
    bool isLimitingNode = false;
    bool isHeadNode = false;
    Node *limitDown;
    Node *limitUp;


    Node(int value);
    Node(bool isLimit, bool isHead);
    void Init(int value);
    ~Node();
};


#endif //TSX_NODE_H
