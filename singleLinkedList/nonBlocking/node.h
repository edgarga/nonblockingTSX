#include <atomic>

class Node{
    public:
        std::atomic<bool> markedToDelete;
        int key;
        std::atomic<Node*> next;

        Node();
        Node(int key);
        ~Node();

};