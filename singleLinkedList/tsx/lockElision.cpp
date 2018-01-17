//
// Created by edgar on 10.01.18.
//

#include "lockElision.h"

unsigned LockElision::startTransaction() {
    unsigned status;
//    for (int i = 0; i < 1000; i++) {
//        if ((status = _xbegin()) == _XBEGIN_STARTED) {
//        }
    status = _xbegin();
    return status;

//        if (status & _XABORT_RETRY){
//
//        }
//}

}

bool LockElision::testTransaction() {
    int returnVal = _xtest();
    return returnVal;
}

bool LockElision::endTransaction() {
    if (_xtest()){

        _xend();
        return true;
    }else{
        return false;
    }
}

LockElision::~LockElision() {
    if (_xtest())
        _xend();
}