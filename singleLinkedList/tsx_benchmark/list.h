//
// Created by edgar on 10.01.18.
//

#ifndef MUTEX_LIST_H
#define MUTEX_LIST_H


#include "node.h"


class List {
public:
    Node *head;
    Node *tail;


    int absoluteTries_Insert;
    int absoluteTries_Delete;

    bool useTsxSearch = false;

    List();

    ~List();

    bool insert(int key);

    bool del(int searchKey);

    Node *search(int key, Node **leftNode);

    Node *tsxSearch(int key, Node **leftNode);

    bool isMarkedPtr(size_t node);

    bool isMarkedPtr(Node *node);

    Node *getMarkedPtr(Node *node);

    Node *getUnmarkedPtr(Node *node);

    bool areMarkedInbetween(Node *start, Node *end);

    void print();
    bool isIncreasing();

    void printStats();
};


#endif //MUTEX_LIST_H
