#include <iostream>
#include "skipList.h"

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

            Node *right = list.searchWithMarked(key, &left, currentHeight);

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
    insert(1, 4, list);
    insert(3, 3, list);
    insert(5, 2, list);
    insert(7, 4, list);

    Node *drei = list.search(3);
    Node *nextOfDrei = drei->next;
    drei->next.compare_exchange_strong(nextOfDrei, list.getMarked(nextOfDrei));
    drei = drei->down;
    nextOfDrei = drei->next;
    drei->next.compare_exchange_strong(nextOfDrei, list.getMarked(nextOfDrei));
    drei = drei->down;
    nextOfDrei = drei->next;
    drei->next.compare_exchange_strong(nextOfDrei, list.getMarked(nextOfDrei));

    print(list);
    int searchKey = 5;
    Node *searchNode = list.search(searchKey);
    print(list);
    std::cout << "searchend: " << searchKey << " | got: " << searchNode->value << std::endl;




//    int inCount =0;
//    for(int i= 0; i< 50; i++){
//        if(list.insert(rand() % 100))
//            inCount++;
//    }
//
//    int delCount =0;
//    for(int i= 0; i< 50; i++){
//        if(list.remove(rand() % 100))
//            delCount++;
//    }
//
//    int inCount1 = 0;
//    Node *cur = list.getUnmarked(list.headRootNode->next);
//    while(cur != list.tailRootNode){
//        cur = list.getUnmarked(cur->next);
//        inCount1++;
//    }
//
//    int oldVal = list.headRootNode->value;
//    bool isAsc = true;
//    cur = list.getUnmarked(list.headRootNode->next);
//    while(cur != list.tailRootNode){
//        if(oldVal >= cur->value){
//            isAsc = false;
//            break;
//        }
//        cur = list.getUnmarked(cur->next);
//    }
//
//    bool removeSucc = list.remove(20);

//    std::cout << "in List: " << inCount1 << " | count: " << inCount - delCount << std::endl;
//    std::cout << "List is in ascending Order: " << (isAsc? "true":"false") << std::endl;
//    std::cout << "20 deleted: " << (removeSucc? "true":"false") << std::endl;


    return 0;
}