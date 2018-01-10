//
// Created by edgar on 10.01.18.
//

#include <immintrin.h>

#ifndef MUTEX_LOCKELISION_H
#define MUTEX_LOCKELISION_H


class LockElision {
public:
    void startTransaction();
    bool testTransaction();
    void endTransaction();
};


#endif //MUTEX_LOCKELISION_H
