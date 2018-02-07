//
// Created by edgar on 10.01.18.
//

#ifndef MUTEX_RTMLOCK_H
#define MUTEX_RTMLOCK_H


#include "lockElision.h"

class RTMLock {
public:
    LockElision elisionLock;
    explicit RTMLock(LockElision elision);
    ~RTMLock();
};


#endif //MUTEX_RTMLOCK_H
