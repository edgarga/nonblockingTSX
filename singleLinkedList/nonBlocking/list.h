#include "node.h"
#include <mutex>


class List {
public:
    Node *head;
    Node *tail;

    List();

    ~List();

    bool insert(int key);

    bool del(int searchKey);

    bool contains(int key);

    bool isMarkedPtr(Node *node);
    Node *getMarkedPtr(Node *node);
    Node *getUnmarkedPtr(Node *node);

    Node *search(int key, Node **leftNode);

    void print();
};