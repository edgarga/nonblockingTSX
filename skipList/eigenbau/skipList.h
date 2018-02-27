//
// Created by edgar on 26.02.18.
//

#ifndef EIGENBAU_SKIPLIST_H
#define EIGENBAU_SKIPLIST_H


#include "node.h"

class SkipList {

public:

    int height;

    Node *headRootNode, *headTop;
    Node *tailRootNode, *tailTop;


    Node *search(int key);
    Node *search(int key, Node **left);
    Node *search(int key, Node **left, int onLebel);

    bool insert(int key);
    bool remove(int key);

    bool isMarked(Node *ptr);
    Node *getMarked(Node *ptr);
    Node *getUnmarked(Node *ptr);

    explicit SkipList(int height);
    ~SkipList();
};


#endif //EIGENBAU_SKIPLIST_H
