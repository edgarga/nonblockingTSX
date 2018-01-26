#include <atomic>



class Node {
public:
    int key;
    std::atomic<Node *> next;

//    Node();

    Node(int key);

    ~Node();

};