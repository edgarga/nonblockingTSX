#include "NodeList.h"
class ListLinked{
    public:
        NodeList *head;
        NodeList *tail;
    
    bool insert(int value);
    bool remove(int value);
    bool includes(int value);
    int pop();
    void print();

    ListLinked();
};


