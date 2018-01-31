#include <iostream>
#include <thread>
#include <vector>
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

int some = 0;
bool useSome = false;

void pushWorker(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        int value = rand() % upperLimit;
        if (useSome)
            value = some;
        if (list.insert(value, threadId)) {
            pushCountArray[threadId]++;

        }
    }
}


void deleteWorker(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        int value = rand() % upperLimit;

        if (useSome)
            value = some;
        if (list.del(value, threadId)) {
            deleteCountArray[threadId]++;
        }

    }
}

void pushWorkerIntense(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        int value = rand() % 10;
        if (list.insert(value, threadId)) {
            pushCountArray[threadId]++;

        }
    }
}


void deleteWorkerIntense(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        int value = rand() % 10;
        if (list.del(value, threadId)) {
            deleteCountArray[threadId]++;
        }

    }
}


std::atomic<int> ceil;
std::atomic<int> floor;

void pushQueueWorker(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        if (floor <= 0) break;
        int pushValue = floor;
        if (list.insert(pushValue, threadId)) {
            pushCountArray[threadId]++;
            floor--;
        }
    }
}

void deleteQueueWorker(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        if (ceil <= 0) break;
        int delValue = ceil;
        if (list.del(delValue, threadId)) {
            deleteCountArray[threadId]++;
            ceil--;
        }

    }
}

std::atomic<int> popFrontPosition;

void pushPopWorker(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        if (popFrontPosition <= 0) break;
        int pushValue = popFrontPosition;
        if (list.insert(pushValue, threadId)) {
            pushCountArray[threadId]++;
            popFrontPosition--;
        }
    }
}

void deletePopWorker(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        if (popFrontPosition <= 0) break;
        int delValue = popFrontPosition;
        if (list.del(delValue, threadId)) {
            deleteCountArray[threadId]++;
            popFrontPosition++;
        }

    }
}



std::atomic<int> popBackPosition;

void pushPopBackWorker(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        if (popBackPosition <= 0) break;
        int pushValue = popBackPosition;
        if (list.insert(pushValue, threadId)) {
            pushCountArray[threadId]++;
            popBackPosition++;
        }
    }
}

void deletePopBackWorker(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        if (popBackPosition <= 0) break;
        int delValue = popBackPosition;
        if (list.del(delValue, threadId)) {
            deleteCountArray[threadId]++;
            popBackPosition--;
        }

    }
}


int main(int numberOfArguments, char *arguments[]) {
    mcp_init(numberOfArguments, arguments);

    upperLimit = num_elements;


    /// start the test
    List list;
    list.insertsByNonBlock = new int[num_threads];
    list.insertsByTSX = new int[num_threads];
    list.deletesByNonBlock = new int[num_threads];
    list.deletesByTSX = new int[num_threads];
    list.triesForSuccessfulInsertTSX = new long long[num_threads];
    list.triesForSuccessfulDeleteTSX = new long long[num_threads];
    list.absoluteInsertTsxTries = new long long [num_elements];
    list.absoluteDeleteTsxTries = new long long [num_elements];
    list.abortedTsxInsertTry = new long long[num_elements];
    list.abortedTsxDeleteTry = new long long[num_elements];

    /// initialize resultChecking arrays
    pushCountArray = new int[num_threads];
    deleteCountArray = new int[num_threads];
    for (int j = 0; j < num_threads; j++) {
        pushCountArray[j] = 0;
        deleteCountArray[j] = 0;
        list.insertsByNonBlock[num_threads] = 0;
        list.insertsByTSX[num_threads] = 0;
        list.deletesByNonBlock[num_threads] = 0;
        list.deletesByTSX[num_threads] = 0;
        list.triesForSuccessfulInsertTSX[num_threads] = 0;
        list.triesForSuccessfulDeleteTSX[num_threads] = 0;
        list.absoluteInsertTsxTries[num_elements] = 0;
        list.absoluteDeleteTsxTries[num_elements] = 0;
        list.abortedTsxInsertTry[num_elements] = 0;
        list.abortedTsxDeleteTry[num_elements] = 0;
    }


    int pushCount = 0;
    int delCount = 0;

    time_start();

    if (nonBlock) {
        list.absoluteTries_Delete = 0;
        list.absoluteTries_Insert = 0;
    }

    if(tsxTries >= 0){
        list.absoluteTries_Insert = tsxTries;
        list.absoluteTries_Delete = tsxTries;
        std::cout << "tsx tries set to: " << list.absoluteTries_Delete << std::endl;
    }

    if(standard){
        useSome = false;
        upperLimit = num_elements / 4;
        std::vector<std::thread> tv;
        for (int i = 0; i < num_threads; i++) {
            tv.push_back(std::thread(pushWorker, std::ref(list), num_elements, i));
            tv.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
        }
        for (auto &t: tv)
            t.join();
    }

    if (queue) {
        ceil = num_elements;
        floor = num_elements;
        std::cout << "ceil: " << ceil << " | floor: " << floor << std::endl;

        std::vector<std::thread> tv;
        for (int i = 0; i < num_threads; i++) {
            tv.push_back(std::thread(pushQueueWorker, std::ref(list), num_elements, i));
            tv.push_back(std::thread(deleteQueueWorker, std::ref(list), num_elements, i));
        }
        for (auto &t: tv)
            t.join();

        list.print();
        std::cout << "ceil: " << ceil << " | floor: " << floor << std::endl;

    }

    if(pop){
        popFrontPosition = num_elements;
        std::vector<std::thread> tv;
        for (int i = 0; i < num_threads; i++) {
            tv.push_back(std::thread(pushPopWorker, std::ref(list), num_elements, i));
            tv.push_back(std::thread(deletePopWorker, std::ref(list), num_elements, i));
        }
        for (auto &t: tv)
            t.join();

//        list.print();
        std::cout << "PopPosition: " << popFrontPosition << std::endl;

    }

    if(pop_back){
        popBackPosition = num_elements;
        std::vector<std::thread> tv;
        for (int i = 0; i < num_threads; i++) {
            tv.push_back(std::thread(pushPopBackWorker, std::ref(list), num_elements, i));
            tv.push_back(std::thread(deletePopBackWorker, std::ref(list), num_elements, i));
        }
        for (auto &t: tv)
            t.join();

//        list.print();
        std::cout << "PopBackPosition: " << popBackPosition << std::endl;

    }

    if (focusFront) {

        useSome = true;

        std::cout << "initializing front test" << std::endl;
        some = num_elements + 100;
        for (int j = 0; j < num_elements; j++) {
            std::vector<std::thread> tv;
            for (int i = 0; i < num_threads; i++) {
                tv.push_back(std::thread(pushWorker, std::ref(list), num_elements, i));
                tv.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
            }
            for (auto &t: tv)
                t.join();
//        if(list.del(some, -1))
//            delCount++;
            some--;
        }
        useSome = false;
        std::cout << "finished front test" << std::endl;
    }

    if (focusBack) {

        useSome = true;

        std::cout << "initializing standard test" << std::endl;

        for (int j = 0; j < num_elements; j++) {
            std::vector<std::thread> tv;
            for (int i = 0; i < num_threads; i++) {
                tv.push_back(std::thread(pushWorker, std::ref(list), num_elements, i));
                tv.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
            }
            for (auto &t: tv)
                t.join();
//        if(list.del(some, -1))
//            delCount++;
            some++;
        }
        useSome = false;
        std::cout << "finished standard test" << std::endl;
    }
    int bla = 0;

    if (doShort) {
        std::vector<std::thread> tv;

        for (int i = 0; i < num_threads; i++) {
            bla++;
            tv.push_back(std::thread(pushWorkerIntense, std::ref(list), num_elements, i));
            tv.push_back(std::thread(deleteWorkerIntense, std::ref(list), num_elements, i));
        }
        for (auto &t: tv)
            t.join();

        std::cout << "insert finished" << std::endl;

//        std::vector<std::thread> t;
//
//        for (int i = 0; i < num_threads; i++) {
//            bla++;
////            t.push_back(std::thread(pushWorkerIntense, std::ref(list), num_elements, i));
//            t.push_back(std::thread(deleteWorkerIntense, std::ref(list), num_elements, i));
//        }
//        for (auto &t: t)
//            t.join();
    }

    if (workbench) {
        list.insert(1, -2);
        list.insert(5, -2);
        Node *eins = list.head->next;
        Node *fuenf = eins->next;

        eins->next = list.getMarkedPtr(fuenf);

        Node *left;
        Node *bla = list.search(10, &left);

//        if (list.getMarkedPtr(eins) > list.head + 1000000)
//            std::cout << "bigger" << std::endl;
//        else
//            std::cout << "smaller" << std::endl;

//        std::cout << "1" << std::endl;
//
//        list.insert(10, -2);
//        std::cout<< (list.del(1,-1)? "deleted": "not deleted")<< std::endl;

    }


    time_stop();
//    list.print();

    /**
     * calculate the successful insert and delete operations
     */
    for (int x = 0; x < num_threads; x++) {
        pushCount += pushCountArray[x];
        delCount += deleteCountArray[x];
    }


    /**
     * traverse the List and count the actual present Nodes
     */
    int count = 0, absCount = 0;
    Node *curr = list.head;
    while (curr->next != list.tail) {
        if (!list.isMarkedPtr(curr->next))
            count++;
        absCount++;
        curr = list.getUnmarkedPtr(curr->next);
    }
    std::cout << std::endl << "absoluteCount: " << absCount << "| " << "Nodes in List: " << count
              << " | countByThreads: " << pushCount - delCount
              << std::endl;
    std::cout << "inserts: " << pushCount << " | deletes: " << delCount << std::endl;
    time_print();


    long long absoluteSuccessfulInsertsByTSX = 0, absoluteSuccessfulDeletesByTSX = 0;
    long long absoluteSuccessfulInsertsByNonBlock = 0, absoluteSuccessfulDeletesByNonBlock = 0;
    long long tsxInsertTriesUntilSuccess = 0, tsxDeleteTriesUntilSuccess = 0;
    long long absoluteDeleteTsxTries = 0, absoluteInsertTsxTries = 0;
    long long absoluteAbortedDeleteTries = 0, absoluteAbortedInsertTries = 0;

    for (int i = 0; i < num_threads; i++) {
        absoluteSuccessfulInsertsByTSX += list.insertsByTSX[i];
        absoluteSuccessfulDeletesByTSX += list.deletesByTSX[i];

        absoluteSuccessfulInsertsByNonBlock += list.insertsByNonBlock[i];
        absoluteSuccessfulDeletesByNonBlock += list.deletesByNonBlock[i];

        tsxInsertTriesUntilSuccess += list.triesForSuccessfulInsertTSX[i];
        tsxDeleteTriesUntilSuccess += list.triesForSuccessfulDeleteTSX[i];

        absoluteInsertTsxTries += list.absoluteInsertTsxTries[i];
        absoluteDeleteTsxTries += list.absoluteDeleteTsxTries[i];

        absoluteAbortedInsertTries += list.abortedTsxInsertTry[i];
        absoluteAbortedDeleteTries += list.abortedTsxDeleteTry[i];
    }


    double a = static_cast<double>(tsxInsertTriesUntilSuccess) / static_cast<double> (absoluteSuccessfulInsertsByTSX);
    double b = static_cast<double>(tsxDeleteTriesUntilSuccess) / static_cast<double> (absoluteSuccessfulDeletesByTSX);

    double tsxTriesPerInsert = static_cast<double>(absoluteInsertTsxTries) / static_cast<double> (pushCount);
    double tsxTriesPerDelete = static_cast<double>(absoluteDeleteTsxTries) / static_cast<double> (delCount);

    double tsxAbortsPerInsert = static_cast<double> (absoluteAbortedInsertTries) / static_cast<double> (pushCount);
    double tsxAbortsPerDelete = static_cast<double> (absoluteAbortedDeleteTries) / static_cast<double> (delCount);

    std::cout << "nonBlock inserts: " << absoluteSuccessfulInsertsByNonBlock << std::endl;
    std::cout << "tsx inserts: " << absoluteSuccessfulInsertsByTSX << std::endl;
    std::cout << "tsx insert tries absolute: " << absoluteInsertTsxTries << std::endl;
    std::cout << "tsx tries per insert: " << tsxTriesPerInsert << std::endl;
    std::cout << "tsx aborted per insert: " << tsxAbortsPerInsert << std::endl;
    std::cout << "tries per successful tsx insert: " << a << std::endl;
    std::cout << std::endl;
    std::cout << "nonBlock deletes: " << absoluteSuccessfulDeletesByNonBlock << std::endl;
    std::cout << "tsx deletes: " << absoluteSuccessfulDeletesByTSX << std::endl;
    std::cout << "tsx delete tries absolute: " << absoluteDeleteTsxTries << std::endl;
    std::cout << "tsx tries per delete: " << tsxTriesPerDelete << std::endl;
    std::cout << "tsx aborted per delete: " << tsxAbortsPerDelete << std::endl;
    std::cout << "tries per successful tsx delete: " << b << std::endl;

    return 0;
}

