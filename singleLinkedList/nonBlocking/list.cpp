#include <iostream>
#include "list.h"

List::List() {
    this->head = new Node(-1);
    this->tail = new Node(-1);
    this->head->next = this->tail;
}

bool List::insert(int key) {
    Node *newNode = new Node(key);
    Node *rightNode, *leftNode;

    do {
        rightNode = this->search(key, &leftNode);
        if ((rightNode != this->tail) && (rightNode->key == key)) {
            return false;
        }
        newNode->next = rightNode;

        if (leftNode->next.compare_exchange_weak(rightNode,
                                                 newNode)) {
            return true;
        }
    } while (true);
}

Node *List::search(int searchKey, Node **leftNode) {
    Node *leftNextNode, *rightNode;

    do {
        Node *t = this->head;
        Node *tNext = this->head->next;
//        1: Find left and right nodes
        do {

            if (!tNext->markedToDelete) {
                (*leftNode) = t;
                leftNextNode = tNext;
            }
            t = tNext;
            if (t == this->tail) break;
            tNext = t->next;
        } while (t->markedToDelete || (t->key < searchKey));
        rightNode = t;
//        2: Check Nodes are adjacent
        if (leftNextNode == rightNode) {
            if ((rightNode != this->tail) && rightNode->markedToDelete) {
                continue;
            } else {
                return rightNode;
            }
        }

        if ((*leftNode)->next.compare_exchange_weak(leftNextNode,
                                                    rightNode)) {
            if ((rightNode != this->tail) && rightNode->markedToDelete) {
                //TODO: delete the node properly
                continue;
            } else {
                return rightNode;
            }
        }

//        contin:;
    } while (true);
}

bool List::contains(int key) {
    Node *rightNode, *leftNode;

    rightNode = search(key, &leftNode);
    if ((rightNode == this->tail) || (rightNode->key != key))
        return false;
    else
        return true;
}

bool List::del(int searchKey) {
    Node *rightNode, *rightNextNode, *leftNode;

    do {

        rightNode = search(searchKey, &leftNode);
//        check if right node is the searched Node
        if ((rightNode == this->tail) || (rightNode->key != searchKey)){
            return false;
        }

        rightNextNode = rightNode->next;

        if (!rightNode->markedToDelete) {
            rightNode->markedToDelete = true;
            if (true) //TODO: CAS (&(right_node.next), rightNextNode, getMarkedReferece(rightNodeNext))
                break;
        }
    } while (true);

//    leftNode->next = rightNextNode; //TODO: delete after the next line is correct
    if (!leftNode->next.compare_exchange_weak(rightNode,
                                              rightNextNode)) //TODO: !CAS (&leftNOde.next), rightNode, rightNextNode)
        rightNode = this->search(searchKey, &leftNode);
    return true;
}

void List::print() {
    Node *currentNode = this->head;

    do {
        if (currentNode == this->head)
            std::cout << "head";
        else
            std::cout << "| " << currentNode->key << " |";

        if (currentNode->next != nullptr)
            std::cout << " -> ";

        currentNode = currentNode->next;
    } while (currentNode->next != nullptr);
    std::cout << "tail";
    std::cout << std::endl;
}

List::~List() {
    std::cout << "deleting List!" << std::endl;
}