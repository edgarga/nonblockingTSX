#include "Node.h"
class LinkedList{
    public:
        Node *head;
        Node *tail;
    
    bool insert(int value);
    bool remove(int value);
    bool includes(int value);
    int pop();
    void print();

    LinkedList();
};


