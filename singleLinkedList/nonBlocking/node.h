#include <atomic>

class Node{
    public:
        bool markedToDelele = false;
        int key;
        std::atomic<Node*> next;

        Node();
        Node(int key);
        ~Node();

};