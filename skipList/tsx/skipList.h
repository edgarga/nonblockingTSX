//
// Created by edgar on 12.02.18.
//

#ifndef TSX_SKIPLIST_H
#define TSX_SKIPLIST_H


#include <atomic>
#include <cstddef>
#include "node.h"

class SkipList {
public:
    std::atomic<Node *> headRoot;
    std::atomic<Node *> tailRoot;
    int maxLevel;

    explicit SkipList(int levelCount);
    ~SkipList();

    Node *getMarkedPtr(Node *node, int markingPosition);
    Node *getMarkedPtr(size_t ptr, int markingPosition);
    Node *getUnmarkedPtr(Node *node, int unmarkingPosition);
    Node *getUnmarkedPtr(size_t ptr, int unmarkingPosition);
    Node *getUnmarkedPtr(Node *node);
    Node *getUnmarkedPtr(size_t ptr);
    bool isMarked(Node *node);
    bool isMarked(size_t ptr);
    bool isMarkedOnPosition(Node *node, int position);
    bool isMarkedOnPosition(size_t ptr, int position);

    Node *buildResult(Node *node, int status, bool success);
    bool isIN(Node *node);
    bool isDeleted(Node *node);
    bool isSuccess(Node *node);
    bool isDuplicateKey (Node *result);

    Node *search(int key, Node **nextNode);
    Node *searchToLevel(int key, int level, Node **node);
    int findStart(int level, Node **resultNode);
    Node *searchRight(int key, Node *currentNode, Node **returnNode);
    Node * tryFlagNode(Node *previousNode, Node *targetNode);

    bool insert(int key);
    Node *insertNode(Node *newNode, Node *prevNode, Node *nextNode, Node **result);

    bool remove(int key);
    bool removeNode(Node *prevNode, Node *delNode);

    bool helpMarked(Node *prevNode, Node *delNode);
    bool helpFlagged(Node *prevNode, Node *delNode);
    void tryMark(Node *node);




};


#endif //TSX_SKIPLIST_H
