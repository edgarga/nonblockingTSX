# include <iostream>
#include <limits>
#include "mcp.h"
#include "node.h"
#include "skipList.h"


bool marked(size_t ptr, int pos) {
    return ptr & (1 << pos);

}


int main(int numberOfArguments, char *arguments[]) {
//    mcp_init(numberOfArguments, arguments);

    SkipList list(5);

    long insertCount = 0;
    long delCount = 0;

    for (int i = 1; i < 10; i++) {
        if (list.insert(i))
            insertCount++;
        if (list.insert(i))
            insertCount++;
        if (list.remove(i))
            delCount++;
        if (list.remove(i))
            delCount++;
    }

    std::cout << "inserts: " << insertCount << " | deletes: " << delCount << std::endl;

    Node *cur = list.headRoot;
    while (cur != nullptr) {

        if (list.isMarkedOnPosition(cur->successor, 1))
            std::cout << " |";

        std::cout << cur->value;

        if (list.isMarkedOnPosition(cur->successor, 0))
            std::cout << "| ";
        std::cout << " -> ";

        cur = list.getUnmarkedPtr(cur->successor);
    }

    std::cout << "NULL";


//    std::cout << "max: " << std::numeric_limits<int>::max() << std::endl
//              << "min: " << std::numeric_limits<int>::min() << std::endl;
//
//
//    Node *newNode = new Node(1);
//    Node *newNode3 = new Node(3);
//    Node *newNode5 = new Node(5);
//    Node *headRoot = list.headRoot;
//    Node *tailRoot = list.tailRoot;
//    headRoot->successor = newNode;
//
//    newNode->successor = newNode3;
//    newNode3->successor = newNode5;
//    newNode5->successor = tailRoot;
//
//
//
//    if (list.remove(1))
//        std::cout << "removed 1" << std::endl;
//    else
//        std::cout << "NOT removed 1" << std::endl;

//    int levelCount = 0;
//    Node *currNode = list.headRoot;
//
//    while (currNode->limitUp != currNode) {
//        levelCount++;
//        currNode = currNode->limitUp;
//    }
//    int s = std::numeric_limits<int>::max();
//    size_t t = 0;
//    for (int i = 0; i < 100; i++) {
//        Node *node = new Node(i);
//        size_t oldPtr = (size_t) node;
//        size_t ptr = (size_t) node;
//        int posOnFirstOne = 0;
//        while (!(ptr & 1)) {
////            std::cout << "ptr: " << ptr << " | ptr & 1: " << (ptr & 1) << " | !(ptr & 1): " << (!(ptr & 1))
////                      << std::endl;
//            posOnFirstOne++;
//            ptr = ptr >> 1;
//
//        }
//        if (posOnFirstOne < s) {
//            s = posOnFirstOne;
//            t = oldPtr;
//        }
//    }
//    std::cout << "current: " << t << " | zeroes: " << s << std::endl;
//
//    std::cout << levelCount << std::endl;
//
//    int test = 16;
//
//    std::cout << (0x10 - 15) << std::endl;
//
//    std::cout << "0 == true: " << (0 == true? "yes": "no") << std::endl;
//    std::cout << "1 == true: " << (1 == true? "yes": "no") << std::endl;


    return 0;
}
