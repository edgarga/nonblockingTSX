//
// Created by edgar on 10.01.18.
//
#include <iostream>
#include <thread>
#include <vector>
#include "lockElision.h"
#include "list.h"
#include "mcp.h"


/**
 * arrays to count successful operations
 */
int *pushCountArray, *deleteCountArray;

/**
 * highest Number that can be present in the List
 */
int upperLimit = 0;


void pushWorker(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        int valToInsert = rand() % upperLimit;
//        std::cout << valToInsert << std::endl;
        if (list.insert(valToInsert)) {
            pushCountArray[threadId]++;
        }
    }
}


void deleteWorker(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        int value = rand() % upperLimit;
        if (list.del(value)) {
            deleteCountArray[threadId]++;
        }
    }
}


int main(int numberOfArguments, char *arguments[]) {
    mcp_init(numberOfArguments, arguments);

    upperLimit = num_elements;

////    initialize resultChecking arrays
    pushCountArray = new int[num_threads];
    deleteCountArray = new int[num_threads];
    for (int j = 0; j < num_threads; j++) {
        pushCountArray[j] = 0;
        deleteCountArray[j] = 0;
    }

    /**
     * start the test
     */
    List list;
    std::vector<std::thread> threadVector;
    time_start();
    for (int i = 0; i < num_threads; i++) {
        threadVector.push_back(std::thread(pushWorker, std::ref(list), num_elements, i));
//        threadVector.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
    }

    for (auto &t: threadVector)
        t.join();

    time_stop();

    /**
     * calculate the successful insert and delete operations
     */
    int pushCount = 0;
    int delCount = 0;
    for (int x = 0; x < num_threads; x++) {
        pushCount += pushCountArray[x];
        delCount += deleteCountArray[x];
    }

    /**
     * traverse the List and count the actual present Nodes
     */
    int count = 0;
    Node *curr = list.head;
    while (curr->next != list.tail) {
        if (!curr->markedToDelete)
            count++;
        curr = curr->next;
    }
    std::cout << "count: " << count << " | countByThreads: " << pushCount - delCount << std::endl;
    std::cout << "inserts: " << pushCount << " | deletes: " << delCount << std::endl;
    time_print();

    unsigned status;
    unsigned end;
    status= _xbegin();
    _xend();

//    _xabort(0xff);
//    std::cout << status << " " << !status  << " " << !(status & _XABORT_RETRY) << " " << (status & _XABORT_RETRY)  << std::endl;
//    if(!(status & _XABORT_RETRY))
//        std::cout << "A"<< std::endl;
//    else
//        std::cout << "B"<< std::endl;



//    list.print();
    return 0;
}

