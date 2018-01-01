#include "LinkedList.h" 

#include <iostream>

    LinkedList::LinkedList(){
        this->head = new Node(-2);
        this->tail = new Node(-1);
        this->head->next = this->tail;
    }

    bool LinkedList::insert(int value){
        Node *newNode = new Node(value);
        // Node *first = this->tail;
        newNode->next = this->head->next;
        this->head->next = newNode;
        return true;
    }

    bool LinkedList::remove(int value){
        return false;
    }

    bool LinkedList::includes(int value){
        return false;
    }

    void LinkedList::print(){
        Node *currentNode = this->head->next;
        std::cout << "head";
        while(currentNode != this->tail){
            std::cout << " -> " << currentNode->value;
            currentNode = currentNode->next;
        }
        std::cout << " -> tail" << std::endl;
    }

    int LinkedList::pop(){
        Node *node = this->head->next;
        int val = node->value;
        delete node;
        this->head->next = node->next;
        return val;
    }
    

    
