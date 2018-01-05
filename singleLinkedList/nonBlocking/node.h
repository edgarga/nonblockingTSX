class Node{
    public:
        bool markedToDelele = false;
        int key;
        Node *next;

        Node();
        Node(int key);
        ~Node();

};