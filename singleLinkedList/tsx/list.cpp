//
// Created by edgar on 10.01.18.
//

#define ABSOLUTE_RETRIES 1000
#define RETRIES_OF_ONE_ITERATION 100

#include "list.h"
#include "rTMLock.h"

List::List() {
    this->head = new Node(-2);
    this->tail = new Node(-1);
    this->head->next = this->tail;
}

bool List::insert(int key) {
    Node *newNode = new Node(key);
    Node *rightNode, *leftNode;

    int absoluteTries = 0;
    bool insertedNewNode = false;
    do {
        int triesOfThisIteration = 0;
        rightNode = this->search(key, &leftNode);
        unsigned status;

        if(rightNode->value == key) return false;

        while (triesOfThisIteration++ < RETRIES_OF_ONE_ITERATION && absoluteTries++ < ABSOLUTE_RETRIES) {
            //start transaction
            //insert as if no other threads are there

            //check if retry is possible or necessary
            //if not => set triesOfThisIteration to max

            //close transaction if necessary

            //check if node was successful inserted

            if ((status = _xbegin()) == _XBEGIN_STARTED) {
                //TODO: normal insert algorithm goes here
            }

            if (!(status & _XABORT_RETRY)) {
                triesOfThisIteration = RETRIES_OF_ONE_ITERATION;
            }

            if (_xtest()) {
                _xend();
            }

            if (!(status & _XABORT_RETRY) && leftNode->next == newNode && newNode->next == rightNode) {
                insertedNewNode = true;
                break;
            }

        }

        if (insertedNewNode) {
            return true;
        } else if (absoluteTries >= ABSOLUTE_RETRIES) {
            //TODO: nonblocking insert algorithm goes here
        }


    } while (true);


}