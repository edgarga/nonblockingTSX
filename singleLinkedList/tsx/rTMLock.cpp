//
// Created by edgar on 10.01.18.
//

#include "rTMLock.h"

RTMLock::RTMLock(LockElision elision): elisionLock(elision) {
    elision.startTransaction();
}

RTMLock::~RTMLock() {
    elisionLock.endTransaction();
}