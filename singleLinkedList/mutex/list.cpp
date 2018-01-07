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
    Node *nextNodeOfLeftNode;

    Node *test = new Node(-204);

    Node *t = this->head;
    Node *tNext = this->head->next;
    do {
        (*leftNode) = t;
        nextNodeOfLeftNode = tNext;
        t = tNext;
        if (t == this->tail) break;
        tNext = t->next;
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
        std::unique_lock<std::mutex> lockLeftNode(leftNode->mtx, std::defer_lock);
        std::unique_lock<std::mutex> lockSuccessorLeftNode(leftNode->next->mtx, std::defer_lock);
        std::lock(lockLeftNode, lockSuccessorLeftNode);
        if (leftNode->next->value == val)
            return false;


        if (leftNode->next != nullptr && ((leftNode->next->value > val && leftNode->next->next != nullptr) || leftNode->next == this->tail)) {
            newNode->next = leftNode->next;
            leftNode->next = newNode;
            return true;
        }

    } while (true);

}

bool List::del(int val) {
    Node *leftNode, *nodeToDelete, *rightNode;

    do {
        nodeToDelete = this->search(val, &leftNode);
        std::unique_lock<std::mutex> lockOfNodeToDelete(nodeToDelete->mtx, std::defer_lock);
        std::unique_lock<std::mutex> lockOfLeftNode(leftNode->mtx, std::defer_lock);

        std::lock(lockOfNodeToDelete, lockOfLeftNode);
        if (nodeToDelete->value != val) return false;

        rightNode = nodeToDelete->next;
        if (leftNode->next != nullptr && nodeToDelete->next != nullptr &&
            (rightNode->next != nullptr || rightNode == this->tail) &&
            leftNode->next == nodeToDelete) {

            leftNode->next = nodeToDelete->next;
            nodeToDelete->next = nullptr;
            return true;
        }

    } while (true);

}

void List::print() {
    Node *currentNode = this->head;

    while (currentNode != this->tail) {
        std::cout << " " << currentNode->value << " " << " -> ";
        currentNode = currentNode->next;
    }
    std::cout << " " << currentNode->value << std::endl;
}

List::~List() {}