#include <atomic>



class Node {
public:
    int key;
    std::atomic<bool *> marked;
    std::atomic<Node *> next;

//    Node();

    Node(int key, bool *ptr);

    ~Node();

};