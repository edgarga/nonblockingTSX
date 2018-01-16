#include "node.h"
#include <mutex>


class List {
public:
    Node *head;
    Node *tail;
    std::atomic<bool *> toTrue;
    std::atomic<bool *> toFalse;

    List();

    ~List();

    bool insert(int key);

    bool del(int searchKey, int threadNum);

    bool contains(int key);

    Node *search(int key, Node **leftNode, int threadId);

    void print();
};