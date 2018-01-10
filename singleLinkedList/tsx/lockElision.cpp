//
// Created by edgar on 10.01.18.
//

#include "lockElision.h"

void LockElision::startTransaction() {
    unsigned status;
    for(int i=0; i < 1000; i++){
        if((status = _xbegin()) == _XBEGIN_STARTED){
            return;
        }

//        if (status & _XABORT_RETRY){
//
//        }
    }
}

bool LockElision::testTransaction() {
    int returnVal = _xtest();
    return returnVal;
}

void LockElision::endTransaction() {
    if(_xtest())
        _xend();
}