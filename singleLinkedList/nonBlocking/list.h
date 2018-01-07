#include "node.h"

class List{
    public:
        Node *head;
        Node* tail;

        List();
        ~List();

        bool insert(int key);
        bool del(int searchKey, int threadId);
        bool contains(int key);
        Node* search(int key, Node **leftNode, int threadId);
        void print();
};