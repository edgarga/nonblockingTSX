//
// Created by edgar on 10.01.18.
//

#include <immintrin.h>

#ifndef MUTEX_LOCKELISION_H
#define MUTEX_LOCKELISION_H


class LockElision {
public:
    unsigned startTransaction();
    bool testTransaction();
    void endTransaction();
    ~LockElision();
};


#endif //MUTEX_LOCKELISION_H
