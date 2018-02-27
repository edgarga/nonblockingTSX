//
// Created by edgar on 26.02.18.
//

#include <glob.h>
#include <limits>
#include <cstdlib>
#include <iostream>
#include "skipList.h"

SkipList::SkipList(int height) {
    this->height = height;

    Node *curHead, *lastHead;
    Node *curTail, *lastTail;

    lastHead = new Node(true, true);
    lastHead->level = 0;
    lastHead->root = lastHead;
    lastTail = new Node(true, false);
    lastTail->level = 0;
    lastTail->root = lastTail;

    lastHead->next = lastTail;

    this->headRootNode = lastHead;
    this->tailRootNode = lastTail;

    int currentLevel = 0;

    while (++currentLevel < height) {
        curHead = new Node(true, true);
        curHead->level = currentLevel;
        curTail = new Node(true, false);
        curTail->level = currentLevel;
        curHead->next = curTail;

        curHead->down = lastHead;
        lastHead->limitNextUp = curHead;
        curTail->down = lastTail;
        lastTail->limitNextUp = curTail;

        lastHead = curHead;
        lastTail = curTail;
    }
    this->headTop = lastHead;
    this->tailTop = lastTail;

}

bool SkipList::insert(int key) {
    int towerHeight = ((rand() % 2) * (rand() % (this->height - 2))) + 1;
    int currentHeight = 0;

    std::cout << " building tower for: " << key << " height: " << towerHeight << std::endl;
    Node *inserted = nullptr;
    Node *rootOfTower = nullptr;

    while (currentHeight < towerHeight) {
        Node *insertingNode = new Node(key);
        insertingNode->down = inserted;
        insertingNode->root = rootOfTower;
        Node *left;
        do {

            Node *right = search(key, &left, currentHeight);

            /// Falls Schlüssel schon in aktueller Ebene vorhanden ODER
            /// root des aktuellen Towers zwischenzeitlich gelöscht wird / wurde
            if (right->value == key || (rootOfTower != nullptr && isMarked(rootOfTower->next)))
                return rootOfTower != nullptr; /// false falls noch nichts eingefügt wurde; true: sonst

            insertingNode->next = right;
            if (left->next.compare_exchange_strong(right, insertingNode)) {

                inserted = insertingNode;
                inserted->level = currentHeight;
                if (rootOfTower == nullptr) rootOfTower = inserted;
                currentHeight++;
                break;
            }
        } while (true);

    }
    return true;
}


bool SkipList::remove(int key) {
    Node *left = nullptr, *del = nullptr, *right = nullptr;
    bool didThisThreadMarkRoot = false;
    bool markedFinished = false;
    bool rootWasDeleted = false;
    do {

        del = search(key, &left);

        if ((del->value != key || del->isLimit) && !didThisThreadMarkRoot) return false;
        Node *root = del->root;


        /// Phase 1: Markieren aller Elemente des Turms
        /// Phase 1.1: Markieren des root elements
        if (!didThisThreadMarkRoot && !markedFinished) {
            Node *nextOfRoot = root->next;
            if (isMarked(nextOfRoot)) return false;
            if (root->next.compare_exchange_strong(nextOfRoot, getMarked(nextOfRoot))) {
                didThisThreadMarkRoot = true;
            } else {
                continue;
            }
            /// Phase 1.2: Markieren des restlichen Turms, von oben nach Unten
        } else if (didThisThreadMarkRoot && !markedFinished) {

            Node *currentMarking = del;
            while (currentMarking != root) {
                Node *nextOfCurr = getUnmarked(currentMarking->next);
                if (del->next.compare_exchange_strong(nextOfCurr, getMarked(nextOfCurr))){
                    currentMarking = currentMarking->down;

                }

            }
            markedFinished = true;
//            right = getUnmarked(del->next);
//            if (del->next.compare_exchange_strong(right, getMarked(right)))
//                if (del->down == nullptr)
//                    markedFinished = true;
            continue;

        } else { /// Phase 2: Löschen des Turms von oben nach unten
            right = getUnmarked(del->next);
            if (left->next.compare_exchange_strong(del, right))
                if (del->down == nullptr)
                    return true;
            continue;
        }
//        /// Phase 1.2: Markieren des restlichen Turms, von oben nach unten
//        Node *currentMarking = del;
//        while (currentMarking != root) {
//            Node *nextOfCurr = getUnmarked(currentMarking->next);
//
//        }

    } while (true);
    return true;
}

Node *SkipList::search(int key) {
    Node *currentNode = this->headTop;
    int currentLevel = currentNode->level;
    while (currentLevel >= 0 && currentNode->value < key) {
        Node *next = currentNode->next;
        if (getUnmarked(next)->value > key) {
            currentNode = currentNode->down;
            currentLevel--;
        } else {
            currentNode = getUnmarked(next);

        }


    }
    return currentNode;
}

Node *SkipList::search(int key, Node **left) {
    return search(key, left, 0);
}


/**
 * sucht von oben nach unten in der skipListe bis maximal zu einer festgelegten Ebene.
 * Wird ein Element mit dem spezifiziertem Schlüssel in einer höheren Ebene gefunden,
 * wird das Element dieser oberen Ebene zurückgeben.
 * @param key Der Schlüssel nach dem gesucht wird
 * @param left der Vorgänger des des Zurückgegebenen Elements, in jedem Fall auf der gleichen Ebene wie das
 * zurückgebene Element
 * @param level das level bis zu dem maximal gesucht werden soll
 * @return falls vorhanden: das Elementen mit dem gesuchten Schlüssel.
 * Sonst: Das Element mit dem nächst größeren Schlüssel
 */
Node *SkipList::search(int key, Node **left, int level) {
    Node *right = headTop;
    int currentLevel = right->level;
    do {
        (*left) = right;
        if (getUnmarked(right->next)->value <= key) {
            right = getUnmarked(right->next);
        } else if (currentLevel > level) {
            right = right->down;
            currentLevel--;
        } else {
            right = getUnmarked(right->next);
            break;
        }
    } while (right->value < key);
    return right;
}

bool SkipList::isMarked(Node *node) {
    size_t ptr = (size_t) node;
    return (ptr & 0x1) != 0;

}

Node *SkipList::getMarked(Node *node) {
    size_t ptr = (size_t) node;
    ptr = ptr | 0x1;
    return (Node *) ptr;
}

Node *SkipList::getUnmarked(Node *node) {
    size_t ptr = (size_t) node;
    ptr = ptr & (std::numeric_limits<size_t>::max() - 0x1);
    return (Node *) ptr;
}

SkipList::~SkipList() {}