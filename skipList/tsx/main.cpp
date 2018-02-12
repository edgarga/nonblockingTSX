#include <iostream>
#include "mcp.h"
#include "node.h"
#include "skipList.h"

bool marked(size_t ptr, int pos) {
    return ptr & (1 << pos);

}


int main(int numberOfArguments, char *arguments[]) {
//    mcp_init(numberOfArguments, arguments);

    SkipList list(5);

    int levelCount = 0;
    Node *currNode = list.headRoot;

    while (currNode->limitUp != currNode) {
        levelCount ++;
        currNode = currNode->limitUp;
    }
    size_t t;
    std::cout << levelCount << std::endl;


    return 0;
}
