#include <iostream>
#include <thread>
#include <vector>
#include "skipList.h"

std::atomic<int> insertCount;
std::atomic<int> removeCount;

void insertWorker(SkipList &list, int numberOfTries, int max, int threadId) {
    for (int i = 0; i < numberOfTries; i++) {
        if (list.insert(rand() % max))
            insertCount++;
    }
}

void removeWorker(SkipList &list, int numberOfTries, int max, int threadId) {
    for (int i = 0; i < numberOfTries; i++) {
        if (list.del(rand() % max))
            removeCount++;
    }
}

bool insert(int key, int levels, SkipList &list) {
    int towerHeight = levels;
    int currentHeight = 0;

    std::cout << " building tower for: " << key << " height: " << towerHeight << std::endl;
    Node *inserted = nullptr;
    Node *rootOfTower = nullptr;

    while (currentHeight < towerHeight) {
        Node *insertingNode = new Node(key);
        insertingNode->down = inserted;
        Node *left;
        do {

            Node *right = list.searchToLevel(key, &left, currentHeight);

            /// Falls Schlüssel schon in aktueller Ebene vorhanden ODER
            /// root des aktuellen Towers zwischenzeitlich gelöscht wird / wurde
            if (right->value == key || (rootOfTower != nullptr && list.isMarked(rootOfTower->next)))
                return rootOfTower != nullptr; /// false falls noch nichts eingefügt wurde; true: sonst

            insertingNode->next = right;
            if (left->next.compare_exchange_strong(right, insertingNode)) {

                inserted = insertingNode;
                inserted->level = currentHeight;
                if (rootOfTower == nullptr) rootOfTower = inserted;
                insertingNode->root = rootOfTower;
                currentHeight++;
                break;
            }
        } while (true);

    }
    return true;
}

void print(SkipList &list) {
    std::cout << std::endl << std::endl;
    Node *currentFront = list.headTop;

    while (currentFront != nullptr) {
        Node *currentNode = currentFront;
        while (currentNode != nullptr) {
            if (currentFront != currentNode) std::cout << " -> ";
            if (list.isMarked(currentNode->next))
                std::cout << "\033[1;31m" << currentNode->value << "." << currentNode->level << "\033[0m";
            else

                std::cout << currentNode->value << "." << currentNode->level;
            currentNode = list.getUnmarked(currentNode->next);
        }
        std::cout << std::endl;
        currentFront = currentFront->down;
    }
}

int main() {
    std::cout << "start" << std::endl;
    SkipList list(5);
//    list.insert(1);
//    list.insert(3);
//    Node *t3;
//    Node *sMark = list.searchTopmost(1, &t3);
//    Node *sToLevel = list.searchToLevel(1, &t3, 0);
//    list.insert(5);
//    list.insert(7);
////    insert(1, 4, list);
////    insert(3, 3, list);
////    insert(5, 2, list);
////    insert(7, 4, list);
//    list.del(3);
//    print(list);
//    list.del(5);
//    Node *leftNode = nullptr;
//    Node *drei = list.search(3);
//    std::cout << "-------- inserted! ---------" << std::endl;
//    print(list);
//    Node *nextOfDrei = drei->next;
//    drei->next.compare_exchange_strong(nextOfDrei, list.getMarked(nextOfDrei));
//    drei = drei->down;
//    nextOfDrei = drei->next;
//    drei->next.compare_exchange_strong(nextOfDrei, list.getMarked(nextOfDrei));
//    drei = drei->down;
//    nextOfDrei = drei->next;
//    drei->next.compare_exchange_strong(nextOfDrei, list.getMarked(nextOfDrei));

//    std::cout << "-------- marked3! ---------" << std::endl;
//    print(list);
//    insert(3, 3, list);
//    list.remove(3);
//    std::cout << "-------- inserted 6! ---------" << std::endl;
//    print(list);

//    int searchKey = 3;
//
//    Node *searchNode;
//    int l = 0;
//    do {
//
//        searchNode = list.search(searchKey, &leftNode, l++);
//        print(list);
//        std::cout << "searchend: " << searchKey << " on level " << l - 1 << " | got: " << searchNode->value << "."
//                  << searchNode->level << std::endl;
//    } while (l < 3);
//    std::cout << "searchend: " << searchKey << " on level " << l - 1 << " | got: " << searchNode->value << "."
//              << searchNode->level << std::endl;


    insertCount = 0;
    removeCount = 0;

    int num_elements = 100000;
    int max = 256;
    insertWorker(list, num_elements, max, 1);
//    removeWorker(list, num_elements, max, 1);
    std::vector<std::thread> tv;

    for (int i = 0; i < 4; i++) {
        tv.push_back(std::thread(insertWorker, std::ref(list), num_elements, max, i));
        tv.push_back(std::thread(removeWorker, std::ref(list), num_elements, max, i));
    }
    for (auto &t: tv)
        t.join();

    int inCount1 = 0;
    Node *cur = list.getUnmarked(list.headRootNode->next);
    while (cur != list.tailRootNode) {
        if (!list.isMarked(cur->next))
            inCount1++;
        cur = list.getUnmarked(cur->next);
    }

    int oldVal = list.headRootNode->value;
    bool isAsc = true;
    cur = list.getUnmarked(list.headRootNode->next);
    while (cur != list.tailRootNode) {
        if (oldVal >= cur->value) {
            isAsc = false;
            break;
        }
        cur = list.getUnmarked(cur->next);
    }


    std::cout << "in List: " << inCount1 << " | count: " << insertCount - removeCount << std::endl;
    std::cout << "inserts: " << insertCount << " | deletes: " << removeCount << std::endl;
    std::cout << "List is in ascending Order: " << (isAsc ? "true" : "false") << std::endl;
//    print(list);


    return 0;
}