#include "ListLinked.h" 

#include <iostream>

    ListLinked::ListLinked(){
        this->head = new NodeList(-2);
        this->tail = new NodeList(-1);
        this->head->next = this->tail;
    }

    bool ListLinked::insert(int value){
        NodeList *newNode = new NodeList(value);
        // Node *first = this->tail;
        newNode->next = this->head->next;
        this->head->next = newNode;
        return true;
    }

    bool ListLinked::remove(int value){
        return false;
    }

    bool ListLinked::includes(int value){
        return false;
    }

    void ListLinked::print(){
        NodeList *currentNode = this->head->next;
        std::cout << "head";
        while(currentNode != this->tail){
            std::cout << " -> " << currentNode->value;
            currentNode = currentNode->next;
        }
        std::cout << " -> tail" << std::endl;
    }

    int ListLinked::pop(){
        NodeList *node = this->head->next;
        int val = node->value;
        delete node;
        this->head->next = node->next;
        return val;
    }

    
