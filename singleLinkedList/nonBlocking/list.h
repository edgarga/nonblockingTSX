#include "node.h"

class List{
    public:
        Node *head;
        Node* tail;

        List();
        ~List();

        bool insert(int key);
        bool del(int key);
        bool contains(int key);
        Node* search(int key, Node **left_node);
};