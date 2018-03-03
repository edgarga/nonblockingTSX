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
    Node *searchToLevel(int key, Node **left, int to);
    Node *searchTopmost(int key, Node **left);
    Node *searchTopmost(int key, Node **left, int onLevel);
//    Node *search(int key, Node **left, int onLevel);

    bool insert(int key);
//    bool remove(int key);
    bool del(int key);

    bool isMarked(Node *ptr);
    Node *getMarked(Node *ptr);
    Node *getUnmarked(Node *ptr);

    bool isPhysicallyDeleted(Node *ptr);
    Node *getDeletedPtr(Node *ptr);

    explicit SkipList(int height);
    ~SkipList();
};


#endif //EIGENBAU_SKIPLIST_H
