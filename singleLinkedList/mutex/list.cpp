//
// Created by edgar on 07.01.18.
//

#include <iostream>
#include <mutex>

#include "list.h"

List::List() {
    this->head = new Node();
    this->tail = new Node(-2);
    this->head->next = this->tail;
}

Node *List::search(int val, Node **leftNode) {
//    Node *nextNodeOfLeftNode;

    Node *t = this->head;
    Node *tNext = this->head->next;

    do {
        (*leftNode) = t;
//        nextNodeOfLeftNode = tNext;
        t = tNext;
        if (t == this->tail) break;

        if (t->next != nullptr) {
            tNext = t->next;
        } else {
            t = this->head;
            tNext = t->next;
        }
    } while (t->value < val);

    return t;
}

Node *List::predecessorOf(int val) {
    Node *currentNode = this->head;

    while (currentNode->next != this->tail && val >= currentNode->next->value) {
        currentNode = currentNode->next;
    }

    return currentNode;
}

bool List::insert(int val) {
    Node *newNode = new Node(val);
    Node *leftNode, *rightNode;
    do {
        rightNode = this->search(val, &leftNode);
//        std::cout << "trying ins: " << leftNode->value << " and " << leftNode->next->value << std::endl;
        std::lock(leftNode->mtx, rightNode->mtx);
        std::lock_guard<std::mutex> lock(leftNode->mtx, std::adopt_lock);
        std::lock_guard<std::mutex> lockNext(rightNode->mtx, std::adopt_lock);
//        std::cout << "locked ins: " << leftNode->value << " and " << leftNode->next->value << std::endl;
        if (rightNode->value == val){
//            leftNode->mtx.unlock();
//            rightNode->mtx.unlock();
            return false;
        }


        if (!leftNode->deleted && leftNode->next == rightNode &&
            ((rightNode->value > val && !rightNode->deleted) ||
                    rightNode == this->tail)) {

            newNode->next = rightNode;
            leftNode->next = newNode;

//            leftNode->mtx.unlock();
//            rightNode->mtx.unlock();
            return true;
        }
//        leftNode->mtx.unlock();
//        rightNode->mtx.unlock();
    } while (true);
}

bool List::del(int val) {
    Node *leftNode, *nodeToDelete, *rightNode;

    do {
        nodeToDelete = this->search(val, &leftNode);
        if (nodeToDelete->value != val) return false;
        rightNode = nodeToDelete->next;
        std::cout << "trying del: " << nodeToDelete->value << " and " << leftNode->value << " and " << nodeToDelete->next->value  << std::endl;
        std::lock(nodeToDelete->mtx, leftNode->mtx, rightNode->mtx);
        std::lock_guard<std::mutex> lockOfNodeToDelete(nodeToDelete->mtx, std::adopt_lock);
        std::lock_guard<std::mutex> lockOfLeftNode(leftNode->mtx, std::adopt_lock);
        std::lock_guard<std::mutex> lockRight(nodeToDelete->next->mtx, std::adopt_lock);
        std::cout << "locked del: " << nodeToDelete->value << " and " << leftNode->value << " and " << nodeToDelete->next->value  << std::endl;



        if(nodeToDelete->deleted){
//            nodeToDelete->mtx.unlock();
//            leftNode->mtx.unlock();
//            rightNode->mtx.unlock();
            return false;
        }

        if (!leftNode->deleted  &&
            !rightNode->deleted &&
            leftNode->next == nodeToDelete &&
                nodeToDelete->next == rightNode) {

            leftNode->next = rightNode;
            nodeToDelete->deleted = true;

//            nodeToDelete->mtx.unlock();
//            leftNode->mtx.unlock();
//            rightNode->mtx.unlock();

            return true;
        }

//        nodeToDelete->mtx.unlock();
//        leftNode->mtx.unlock();
//        rightNode->mtx.unlock();

    } while (true);

}

bool List::print() {
    Node *currentNode = this->head;
    int oldVal = -20;
    bool isAsc = true;
    while (currentNode != this->tail) {
        if (oldVal >= currentNode->value)
            isAsc = true;
        std::cout << " " << currentNode->value << " " << " -> ";
        currentNode = currentNode->next;
        oldVal = currentNode->value;
    }
    std::cout << " " << currentNode->value << std::endl;
    return isAsc;
}

List::~List() {}