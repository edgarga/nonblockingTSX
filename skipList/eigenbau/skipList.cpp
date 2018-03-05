//
// Created by edgar on 26.02.18.
//

#include <glob.h>
#include <limits>
#include <cstdlib>
#include <iostream>
#include "skipList.h"

void SkipList::init() {

    this->tsxInsertCount = 0;
    this->tsxRemoveCount = 0;

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
        curHead->root = this->headRootNode;
        lastHead->limitNextUp = curHead;
        curTail->down = lastTail;
        curTail->root = this->tailRootNode;
        lastTail->limitNextUp = curTail;

        lastHead = curHead;
        lastTail = curTail;
    }
    this->headTop = lastHead;
    this->tailTop = lastTail;
}

SkipList::SkipList(int height) {
    this->height = height;
    init();
    this->maxTsxTries = 3;
}

SkipList::SkipList(int height, int tsxTries) {
    this->height = height;
    this->maxTsxTries = tsxTries;
    init();

}

bool SkipList::insert(int key) {
    int towerHeight = (rand() % (this->height - 1)) + 1;
    int currentHeight = 0;
    bool insertedSomething = false;
    int unseccussfulTsxTries = 0;

    Node *inserted = nullptr;
    Node *rootOfTower = nullptr;


    while (currentHeight < towerHeight) {
        Node *insertingNode = new Node(key);
        insertingNode->down = inserted;
        Node *left;
        do {
//            bool insertedByTsx = false;
            Node *right = searchToLevel(key, &left, currentHeight);

            insertingNode->next = right;
            insertingNode->level = currentHeight;
            insertingNode->root = (rootOfTower != nullptr ? rootOfTower : insertingNode);


            if (unseccussfulTsxTries++ < this->maxTsxTries) {
                unsigned status;
                LockElision elision;
                if ((status = elision.startTransaction()) == _XBEGIN_STARTED) {



                    if (right->value == key || (rootOfTower != nullptr && isMarked(rootOfTower->next)))
                        return insertedSomething; /// false falls noch nichts eingefügt wurde; true: sonst
                    if(isMarked(left->next)){
                        unseccussfulTsxTries--;
                        continue;
                    }

                    left->next = insertingNode;
                    insertedSomething = true;
//                    insertedByTsx = true;
                    inserted = insertingNode;
                    currentHeight++;
                    if (rootOfTower == nullptr) rootOfTower = insertingNode;
                }
                if ((elision.endTransaction() &&
                     (status == _XBEGIN_STARTED || status == 0))) {
                    unseccussfulTsxTries--;
                    if (currentHeight == towerHeight){
                        this->tsxInsertCount++;
                        return true;
                    }
                    break;
                }

                if (status & _XABORT_RETRY) {

                } else {
                    continue;
                }
            }

            /// Falls Schlüssel schon in aktueller Ebene vorhanden ODER
            /// root des aktuellen Towers zwischenzeitlich gelöscht wird / wurde
            if (right->value == key || (rootOfTower != nullptr && isMarked(rootOfTower->next)))
                return insertedSomething; /// false falls noch nichts eingefügt wurde; true: sonst


            if (left->next.compare_exchange_strong(right,
                                                   insertingNode)) { //TODO: zweiWord CAS notwendig (darf nur eingesetzt werden wenn auch root.next unmarked ist)
                insertedSomething = true;
                inserted = insertingNode;
                if (rootOfTower == nullptr) rootOfTower = inserted;
                currentHeight++;
                break;
            }
        } while (true);

    }
    if (isMarked(rootOfTower->next) && !isMarked(inserted->next)) {
        Node *cur = inserted;
        while (!isMarked(cur->next)) {
            Node *n = cur->next;
            if (cur->next.compare_exchange_strong(n, getMarked(n)))
                cur = cur->down;
        }
    }
    if (isMarked(rootOfTower->next) && !isMarked(inserted->next))
        std::cout << "snh" << std::endl;
    return true;
}

bool SkipList::remove(int key, int threadId) {
    Node *left = nullptr, *del = nullptr;
    int tsxTries = 0;

    Node *root;

    while (tsxTries < this->maxTsxTries) {
        del = searchTopmost(key, &left, 0);

        while (tsxTries++ < this->maxTsxTries) {
            unsigned status;
            Node *currentMarkingNode;
            LockElision elision;
            if (del->value != key || del->isLimit) return false;

            if ((status = elision.startTransaction()) == _XBEGIN_STARTED) {
                root = del->root;
                if (isMarked(root->next)) return false;

                currentMarkingNode = del;
                while (currentMarkingNode != nullptr) {
                    currentMarkingNode->next = getMarked(currentMarkingNode->next);
                    currentMarkingNode = currentMarkingNode->down;
                }
            }

            if ((elision.endTransaction() && (status == _XBEGIN_STARTED || status == 0))) {
                this->tsxRemoveCount++;
                return true;
            }

            if (status & _XABORT_RETRY) {

            } else {
                break; /// new search will be initiated
            }
        }
    }

    del = searchTopmost(key, &left, 0);
    if (del->value != key || del->isLimit) return false;
    root = del->root;

    /// Phase 1: Markieren aller Elemente des Turms
    /// Phase 1.1: Markieren des root elements
    while (true) {

        Node *nextOfRoot = root->next;
        Node *unmarkedNextOfRoot = getUnmarked(nextOfRoot);
        Node *markedNextOfRoot = getMarked(nextOfRoot);
        if (isMarked(nextOfRoot)) return false;
        if (root->next.compare_exchange_strong(unmarkedNextOfRoot, markedNextOfRoot)) {
            if (!isMarked(root->next))
                std::cout << "nein nein nein!" << std::endl;
            break;
        }
    }

    /// Phase 1.2: Markieren des restlichen Turms, von oben nach Unten

    Node *currentMarking = del;
    while (currentMarking != root) {
        Node *nextOfCurr = currentMarking->next;
        if (isMarked(nextOfCurr) ||
            currentMarking->next.compare_exchange_strong(nextOfCurr, getMarked(nextOfCurr))) {
            currentMarking = currentMarking->down;
        }

    }
    return true;
}

Node *SkipList::searchToLevel(int key, Node **leftNode, int toL) {
    Node *leftNextNode = nullptr, *rightNode = nullptr;
    int levelTo = toL;
//        1: Find left and right nodes
    do {
        Node *t = this->headTop;
        Node *tNext = this->headTop->next;
        (*leftNode) = this->headTop;
        int currentLevel = t->level;


        do {

            if (getUnmarked(tNext)->value > key && currentLevel > levelTo) {
                t = (*leftNode)->down;
                tNext = t->next;
                (*leftNode) = t;
                leftNextNode = tNext;
                currentLevel--;
            } else {

                if (!isMarked(tNext)) {
                    (*leftNode) = t;
                    leftNextNode = tNext;
                }
                t = getUnmarked(tNext);
                if (t->isLimit) {
                    if (currentLevel == levelTo)
                        break;
                    else
                        continue;
                }
                tNext = t->next;
            }
        } while ((isMarked(tNext) || (t->value < key)) || currentLevel > levelTo);
        rightNode = t;

        //        2: Check Nodes are adjacent
        if (leftNextNode == rightNode) {
            if ((!rightNode->isLimit) && isMarked(rightNode->next)) {
                continue;
            } else {
                return rightNode;
            }
        }
        if ((*leftNode)->next.compare_exchange_strong(leftNextNode,
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
Node *SkipList::searchTopmost(int key, Node **leftNode, int onLevel) {
    Node *leftNextNode = nullptr, *rightNode = nullptr;
//    Node *left;
//    Node **leftNode = &left;
    int levelTo = onLevel;
//        1: Find left and right nodes
    do {
        Node *t = this->headTop;
        Node *tNext = this->headTop->next;
        (*leftNode) = this->headTop;
        int currentLevel = t->level;
        do {


            if (getUnmarked(tNext)->value > key && currentLevel > levelTo) {
                t = (*leftNode)->down;
                tNext = t->next;
                (*leftNode) = t;
                leftNextNode = tNext;
                currentLevel--;
            } else {


                if (!isMarked(tNext)) {
                    (*leftNode) = t;
                    leftNextNode = tNext;
                }
                t = getUnmarked(tNext);
                if (t->isLimit) {
                    if (currentLevel == levelTo)
                        break;
                    else
                        continue;
                }
                tNext = t->next;
            }
        } while ((isMarked(tNext) || (t->value < key)));
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

bool SkipList::isMarked(size_t node) {
    return (node & 1) != 0;
}

bool SkipList::isMarked(Node *node) {
    return isMarked((size_t) node);
}

/**
 * Marks the pointer to the node object by setting the smallest bit
 * @param node a pointer to a Node object
 * @return returns node | 1 if node is not marked, node otherwise
 */
Node *SkipList::getMarked(Node *node) {
    size_t ptr = (size_t) node;
    ptr = ptr | 1;
    return (Node *) ptr;

}

/**
 * Unmakrs the pointer ot he Node object be unsetting the smallest bit
 * @param node a pointer to a Node object
 * @return returns the pointer with the least bit not set
 */
Node *SkipList::getUnmarked(Node *node) {
    size_t ptr = (size_t) node;
    ptr = ptr & (std::numeric_limits<std::size_t>::max() - 1);
    return (Node *) ptr;
}

//bool SkipList::isMarked(Node *node) {
//    size_t ptr = (size_t) node;
//    return (ptr & 1) != 0;
//
//}
//
//Node *SkipList::getMarked(Node *node) {
//    size_t ptr = (size_t) node;
//    ptr = ptr | 1;
//    return (Node *) ptr;
//}
//
//Node *SkipList::getUnmarked(Node *node) {
//    size_t ptr = (size_t) node;
//    ptr = ptr & (std::numeric_limits<size_t>::max() - 1);
//    return (Node *) ptr;
//}

SkipList::~SkipList() {}