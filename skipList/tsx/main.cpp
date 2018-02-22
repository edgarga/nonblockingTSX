# include <iostream>
#include <limits>
#include <thread>
#include <vector>
#include "mcp.h"
#include "node.h"
#include "skipList.h"

void print(SkipList list){
    int count = 0;
    Node *cur = list.headRoot;
    while (cur != nullptr) {

        if (list.isMarkedOnPosition(cur->successor, 1))
            std::cout << " |";

        std::cout << cur->value;

        if (list.isMarkedOnPosition(cur->successor, 0))
            std::cout << "| ";
        std::cout << " -> ";

        cur = list.getNode(cur->successor);
        count++;
    }

    std::cout << "NULL" << std::endl;
}

std::atomic<int> insertCount;

std::atomic<int> deleteCount;


void insertWorker(SkipList &list, int pushCount, int threadId) {
    for (int i = 0; i < pushCount; i++) {
        if (list.insert(rand() % 10))
            insertCount++;
    }
}

void deleteWorker(SkipList &list, int delCount, int threadId) {
    for (int i = 0; i < delCount; i++) {
        if (list.remove(rand() % 10, threadId))
            deleteCount++;
    }
}


int main(int numberOfArguments, char *arguments[]) {
    mcp_init(numberOfArguments, arguments);

    insertCount = 0;
    deleteCount = 0;

    SkipList list(5);
    list.print();
    list.insert(0);
    list.print();
    list.remove(0, 1);
    list.print();




//    for (int i = 0; i < 100; ++i) {
//        list.insert(i);
//
//    }

    Node *prev, *delNode;
    prev = list.searchToLevel(5 - 1, 1, &delNode, -7);
    std::cout << "prev: " << list.getNode(prev)->value << " -> " << "del: " << list.getNode(delNode)->value
              << std::endl;
//    list.remove(8);
    std::vector<std::thread> tv;

    size_t ptr = 0x7f08a027ffd1;
    Node *node = (Node *) ptr;
    std::cout << std::hex << (size_t) node << " is Marked: " << (list.isMarked(node) ? "true" : "false")
              << " unmarked: " << list.getNode(node) << " | unmarked is marked=: "
              << (list.isMarked(list.getNode(node)) ? "true" : "false") << std::dec << std::endl;


    for (int i = 0; i < num_threads; i++) {
        tv.push_back(std::thread(insertWorker, std::ref(list), num_elements, i));
        tv.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
    }
    for (auto &t: tv)
        t.join();

    std::cout << "inserts: " << insertCount << " | deletes: " << deleteCount << std::endl;


//    long insertCount = 0;
//    long delCount = 0;
//
//    for (int i = 1; i <= 10; i++) {
//        if (list.insert(i))
//            insertCount++;
////        if (list.insert(i))
////            insertCount++;
////        if (list.remove(i))
////            delCount++;
////        if (list.remove(i))
////            delCount++;
//    }
//
//    std::cout << "inserts: " << insertCount << " | deletes: " << delCount << std::endl;
//
    int count = 0;
    Node *cur = list.headRoot;
    while (cur != nullptr) {

//        if (list.isMarkedOnPosition(cur->successor, 1))
//            std::cout << " |";
//
//        std::cout << cur->value;
//
//        if (list.isMarkedOnPosition(cur->successor, 0))
//            std::cout << "| ";
//        std::cout << " -> ";
//
        cur = list.getNode(cur->successor);
        count++;
    }

    std::cout << "NULL" << std::endl;

    std::cout << "inListByThreads: " << insertCount - deleteCount << " ¦ inList: " << count - 2 << std::endl;


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


