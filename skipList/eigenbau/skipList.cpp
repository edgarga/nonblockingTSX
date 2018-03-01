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
        Node *left;
        do {

            Node *right = searchWithMarked(key, &left, currentHeight);

            /// Falls Schlüssel schon in aktueller Ebene vorhanden ODER
            /// root des aktuellen Towers zwischenzeitlich gelöscht wird / wurde
            if (right->value == key || (rootOfTower != nullptr && isMarked(rootOfTower->next)))
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


bool SkipList::remove(int key) {
    Node *left = nullptr, *del = nullptr, *right = nullptr;
    bool didThisThreadMarkRoot = false;
    bool markedFinished = false;
    bool rootWasDeleted = false;
    do {

        del = searchWithMarked(key, &left);

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

        } else if (didThisThreadMarkRoot &&
                   !markedFinished) {/// Phase 1.2: Markieren des restlichen Turms, von oben nach Unten

            Node *currentMarking = del;
            while (currentMarking != root) {
                Node *nextOfCurr = getUnmarked(currentMarking->next);
                if (isMarked(nextOfCurr) ||
                    currentMarking->next.compare_exchange_strong(nextOfCurr, getMarked(nextOfCurr))) {
                    currentMarking = currentMarking->down;

                }

            }
            markedFinished = true;
            continue;

        } else { /// Phase 2: Löschen des Turms von oben nach unten
            if (isPhysicallyDeleted(del->next)) continue;

            right = getUnmarked(del->next);

            if (left->next.compare_exchange_strong(del, right)) {

                while (true) { /// marking the physically deleted note as such
                    Node *next = del->next;
                    if (isPhysicallyDeleted(next) || del->next.compare_exchange_strong(next, getDeletedPtr(next)))
                        break;
                }

                if (del->down == nullptr)
                    return true;
            }
            continue;
        }

    } while (true);
}

Node *SkipList::search(int key) {
    Node *leftNextNode = nullptr, *rightNode = nullptr;
    Node *left;
    Node **leftNode = &left;
    (*leftNode) = this->headTop;
    int currentLevel = (*leftNode)->level;
    int levelTo = 0;
//        1: Find left and right nodes
    do {
        Node *t = this->headTop;
        Node *tNext = this->headTop->next;
        do {

            if (!isMarked(getUnmarked(tNext)->next) && getUnmarked(tNext)->value > key && currentLevel > 0) {
                t = (*leftNode)->down;
                tNext = t->next;
                currentLevel--;
            }


            if (!isMarked(tNext)) {
                (*leftNode) = t;
                leftNextNode = tNext;
            }
            t = getUnmarked(tNext);
            if (t->isLimit)
                if (currentLevel == 0)
                    break;
                else
                    continue;
            tNext = t->next;
        } while ((isMarked(tNext) || (t->value < key)) && currentLevel >= levelTo);
        rightNode = t;

        //        2: Check Nodes are adjacent
        if (leftNextNode == rightNode) {
            if ((!rightNode->isLimit) && isMarked(rightNode->next)) {
                continue;
            } else {
                return rightNode;
            }
        }
        if ((*leftNode)->next.compare_exchange_weak(leftNextNode,
                                                    rightNode)) {
            if ((!rightNode->isLimit) && isMarked(rightNode)) {
                //TODO: delete the node properly
//                delete rightNode;
                continue;
            } else {
                return rightNode;
            }
        }
    } while (true);

//    Node *currentNode = this->headTop;
//    int currentLevel = currentNode->level;
//    while (currentLevel >= 0 && currentNode->value < key) {
//        Node *next = currentNode->next;
//        if (getUnmarked(next)->value > key) {
//            currentNode = currentNode->down;
//            currentLevel--;
//        } else {
//            currentNode = getUnmarked(next);
//
//        }
//
//
//    }
//    return currentNode;
}

Node *SkipList::searchWithMarked(int key, Node **left) {
    return searchWithMarked(key, left, 0);
}


/**
 * sucht von oben nach unten in der skipListe bis maximal zu einer festgelegten Ebene.
 * Wird ein Element mit dem spezifiziertem Schlüssel in einer höheren Ebene gefunden,
 * wird das Element dieser oberen Ebene zurückgeben.
 * @param key Der Schlüssel nach dem gesucht wird
 * @param left der Vorgänger des des Zurückgegebenen Elements, in jedem Fall auf der gleichen Ebene wie das
 * zurückgebene Element
 * @param onLevel das level bis zu dem maximal gesucht werden soll
 * @return falls vorhanden: das Elementen mit dem gesuchten Schlüssel.
 * Sonst: Das Element mit dem nächst größeren Schlüssel
 */
Node *SkipList::searchWithMarked(int key, Node **left, int onLevel) {
    Node *right = headTop;
    int currentLevel = right->level;
    do {
        (*left) = right;
        if (getUnmarked(right->next)->value <= key) {
            right = getUnmarked(right->next);
        } else if (currentLevel > onLevel) {
            right = right->down;
            currentLevel--;
        } else {
            right = getUnmarked(right->next);
            break;
        }
    } while (right->value < key);
    return right;
}

Node *SkipList::search(int key, Node **leftNode, int onLevel) {
    Node *leftNextNode = nullptr, *rightNode = nullptr;
    int i = 0;
    do {
        Node *t = this->headTop;
        Node *tNext = this->headTop->next;
//        1: Find left and right nodes
        do {
            i++;


            if (!isMarked(tNext)) {
                (*leftNode) = t;
                leftNextNode = tNext;
            }
            t = getUnmarked(tNext);
            if (t->isLimit)
                break;
            tNext = t->next;
        } while (isMarked(tNext) ||
                 (t->value < key));
        rightNode = t;

//        2: Check Nodes are adjacent
        if (leftNextNode == rightNode) {
            if ((!rightNode->isLimit) && isMarked(rightNode->next)) {
                continue;
            } else {
                return rightNode;
            }
        }
        if ((*leftNode)->next.compare_exchange_weak(leftNextNode,
                                                    rightNode)) {
            if ((!rightNode->isLimit) && isMarked(rightNode)) {
                //TODO: delete the node properly
//                delete rightNode;
                continue;
            } else {
                return rightNode;
            }
        }

    } while (true);
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
    ptr = ptr & (std::numeric_limits<size_t>::max() - 0xF);
    return (Node *) ptr;
}

bool SkipList::isPhysicallyDeleted(Node *node) {
    size_t ptr = (size_t) node;
    return (ptr & 0x2) != 0;

}

Node *SkipList::getDeletedPtr(Node *node) {
    size_t ptr = (size_t) node;
    ptr = ptr | 0x2;
    return (Node *) ptr;
}

SkipList::~SkipList() {}