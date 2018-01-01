#include <shared_mutex>

class Node{
    public:
        int value;
        Node *next;
        std::shared_mutex mtex;
        void doSomething();
        bool checkLock();
        Node();
        Node(int value);
        ~Node();
        
};
