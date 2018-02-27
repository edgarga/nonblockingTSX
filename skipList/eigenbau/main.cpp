#include <iostream>
#include "skipList.h"

int main() {
    std::cout << "start" << std::endl;
    SkipList list(5);
//    Node *newNode = new Node(0);
//    newNode->level = 0;
//    Node *newNode1 = new Node(0);
//    newNode1->level = 1;
//    newNode1->down = newNode;
//
//    newNode1->next = list.tailRootNode->limitNextUp;
//    list.headRootNode->limitNextUp->next = newNode1;
//
//    newNode->next = list.tailRootNode;
//    list.headRootNode->next = newNode;
//
//    Node *n1 = new Node(1);
//    n1->next = list.tailRootNode;
//    newNode->next = n1;
//
//
//    Node *s = list.search(0);
//    Node *left;
//    Node *right = list.search(5, &left, 0);

    bool insertSuccess = list.insert(20);
    bool removeSucc = list.remove(20);

    return 0;
}