//
// Created by edgar on 26.02.18.
//

#ifndef EIGENBAU_NODE_H
#define EIGENBAU_NODE_H


#include <atomic>

class Node {

public:
    std::atomic<Node *>next;
    Node *root;
    Node *down;
    int value;

    /// Attributes for Limiting ([Head|Tail]) Nodes
    bool isLimit;
    bool isHeadNode;
    Node *limitNextUp;
    int level;

    void Init();
    explicit Node(int key);
    Node(bool isLimitingNode, bool isHeadNode);
    ~Node();
};


#endif //EIGENBAU_NODE_H
