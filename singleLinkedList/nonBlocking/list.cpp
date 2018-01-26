#include <iostream>
#include "list.h"

List::List() {
    this->head = new Node(-3);
    this->tail = new Node(-1);
    this->head->next = this->tail;
}

/**
 * Inserts a new Node with the specified Value
 * @param key
 * @return true if insertion was successful, false otherwise (i.e. because key already exists)
 */
bool List::insert(int key) {
    Node *newNode = new Node(key);

    Node *rightNode, *leftNode;

    do {
        rightNode = this->search(key, &leftNode);

        if ((rightNode != this->tail) && (rightNode->key == key)) {
            return false;
        }
        newNode->next = rightNode;

        if (leftNode->next.compare_exchange_weak(rightNode, newNode)) {
            return true;
        }
    } while (true);
}

bool List::isMarkedPtr(Node *node) {
    if (node < this->head) {
        return true;
    } else {
        return false;
    }
}

Node *List::getMarkedPtr(Node *node) {
    Node *returnNode = node;
    if (isMarkedPtr(returnNode)) {
        return returnNode;
    } else {
        returnNode -= 0x006000000000;
        return returnNode;
    }
}

Node *List::getUnmarkedPtr(Node *node) {
    Node *returnNode = node;
    if (!isMarkedPtr(returnNode)) {
        return returnNode;
    } else {
        returnNode += 0x006000000000;
        return returnNode;
    }
}

/**
 * searches the the Node with the specified key and deletes marked Nodes
 * @param searchKey
 * @param leftNode
 * @return if key exists: the Node with the specified key, tail otherwise
 */
Node *List::search(int searchKey, Node **leftNode) {
    Node *leftNextNode, *rightNode;

    do {
        Node *t = this->head;
        Node *tNext = this->head->next;
//        1: Find left and right nodes
        do {
            if (!isMarkedPtr(tNext)) {
                (*leftNode) = t;
                leftNextNode = tNext;
            }
            t = getUnmarkedPtr(tNext);
            if (t == this->tail) break;
            tNext = t->next;
        } while (isMarkedPtr(t) || (t->key < searchKey));
        rightNode = t;

//        2: Check Nodes are adjacent
        if (leftNextNode == rightNode) {
            if ((rightNode != this->tail) && isMarkedPtr(rightNode->next)) {
                continue;
            } else {
                return rightNode;
            }
        }
        if ((*leftNode)->next.compare_exchange_weak(leftNextNode,
                                                    rightNode)) {
            if ((rightNode != this->tail) && isMarkedPtr(rightNode)) {
                //TODO: delete the node properly
                continue;
            } else {
                return rightNode;
            }
        }

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

/**
 * marks the Node with the specified key
 * @param searchKey
 * @return true if node marked successfully, false otherwise (i.e. because the key does not exist in the List)
 */
bool List::del(int searchKey) {
    Node *rightNode, *rightNextNode, *leftNode;

    do {
        rightNode = search(searchKey, &leftNode);
        if ((rightNode == this->tail) || (rightNode->key != searchKey)) {
            return false;
        }
        rightNextNode = rightNode->next;

        if (!isMarkedPtr(rightNextNode)) {
            if (rightNode->next.compare_exchange_weak(rightNextNode, getMarkedPtr(rightNextNode)))
                break;
        }
    } while (true);
    if (!(leftNode->next.compare_exchange_weak(rightNode, rightNextNode))) {
        rightNode = this->search(searchKey, &leftNode);
    }

    return true;
}

/**
 * prints the list and all its Nodes
 */
void List::print() {
    Node *currentNode = this->head;

    do {
        if (currentNode == this->head)
            std::cout << "head";
        else
            std::cout << "| " << currentNode->key << " |";

        if (currentNode->next != nullptr)
            std::cout << " -> ";


        currentNode = getUnmarkedPtr(currentNode->next);
    } while (currentNode->next != nullptr);
    std::cout << "tail";
    std::cout << std::endl;
}

List::~List() {
    std::cout << "deleting List!" << std::endl;
}