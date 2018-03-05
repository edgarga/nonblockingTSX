//
// Created by edgar on 26.02.18.
//

#ifndef EIGENBAU_SKIPLIST_H
#define EIGENBAU_SKIPLIST_H


#include "node.h"

class SkipList {

public:

    int height;
    int maxTsxTries;

    std::atomic<int> tsxInsertCount;
    std::atomic<int> tsxRemoveCount;

    Node *headRootNode, *headTop;
    Node *tailRootNode, *tailTop;

    Node *search(int key);

    Node *searchToLevel(int key, Node **left, int to);
    Node *searchTopmost(int key, Node **left, int onLevel);

    bool insert(int key);
    bool remove(int key, int threadId);

    bool isMarked(size_t node);
    bool isMarked(Node *ptr);
    Node *getMarked(Node *ptr);
    Node *getUnmarked(Node *ptr);

    void init();
    explicit SkipList(int height);
    explicit SkipList(int height, int tsxTries);

    ~SkipList();
};


#endif //EIGENBAU_SKIPLIST_H
