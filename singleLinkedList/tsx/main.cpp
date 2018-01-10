//
// Created by edgar on 10.01.18.
//
#include <iostream>
#include "lockElision.h"

int main(int numOfArgs, char* args[]){
    LockElision elision;
    int isLocked = -1;
    elision.startTransaction();
    isLocked = _xtest();
    elision.endTransaction();

    std::cout << "ergebnis: " << isLocked << std::endl;
    return 0;
}
