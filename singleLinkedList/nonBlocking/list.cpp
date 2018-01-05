#include "list.h"

List::List(){
    this->head = new Node(-1);
    this->tail = new Node(-1);
}

bool List::insert(int key){
    Node *newNode = new Node(key);
    Node *rightNode, *leftNode;

    do {
        rightNode = this->search(key, &leftNode);
        if ((rightNode != this->tail) && (rightNode->key == key))
            return false;
    }while(true);

    return true;
}