#include <iostream>
#include <thread>
#include <vector>
#include <math.h>
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

std::atomic<int> ceilN;
std::atomic<int> floorN;

void prepareListForTest(List &list, int numberThreads);

void doStandardTest(List &list, int numberElements, int numThreads);

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


void pushQueueWorker(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        if (floorN <= 0) break;
        int pushValue = floorN;
        if (list.insert(pushValue, threadId)) {
            pushCountArray[threadId]++;
            floorN--;
        }
    }
}

void deleteQueueWorker(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        if (ceilN <= 0) break;
        int delValue = ceilN;
        if (list.del(delValue, threadId)) {
            deleteCountArray[threadId]++;
            ceilN--;
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
    prepareListForTest(list, num_elements);


    int pushCount = 0;
    int delCount = 0;

    time_start();

    if (useTsxSearch) {
        list.useTsxSearch = true;
    }



    if (nonBlock) {
        list.absoluteTries_Delete = 0;
        list.absoluteTries_Insert = 0;
    }


    if (tsxTries >= 0) {
        list.absoluteTries_Insert = tsxTries;
        list.absoluteTries_Delete = tsxTries;
        std::cout << "tsx tries set to: " << list.absoluteTries_Delete << std::endl;
    }

    if (standard) {
        doStandardTest(list, num_elements, num_threads);
    }

    if (queue) {
        ceilN = num_elements;
        floorN = num_elements;
        std::cout << "ceil: " << ceilN << " | floor: " << floorN << std::endl;

        std::vector<std::thread> tv;
        for (int i = 0; i < num_threads; i++) {
            tv.push_back(std::thread(pushQueueWorker, std::ref(list), num_elements, i));
            tv.push_back(std::thread(deleteQueueWorker, std::ref(list), num_elements, i));
        }
        for (auto &t: tv)
            t.join();

        std::cout << "ceil: " << ceilN << " | floor: " << floorN << std::endl;

    }

    if (pop) {
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

    if (pop_back) {
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

    if (doShort) {
        std::vector<std::thread> tv;

        for (int i = 0; i < num_threads; i++) {
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
        for (int i = 0; i <= 20; i += 2) {
            list.insert(i, -2);
        }
        Node *right, *left;
        right = list.tsxSearch(18, &left);
        right->next = list.getMarkedPtr(right->next);
        right = list.tsxSearch(16, &left);
        right->next = list.getMarkedPtr(right->next);
        list.print();
        right = list.tsxSearch(15, &left);

        std::cout << left->key << " -> " << right->key << " : "
                  << (list.areMarkedInbetween(left, right) ? "true" : "false") << std::endl;

        list.insert(15, 0);
        std::cout << "inserting 1" << std::endl;
        list.insert(10, 0);



//        Node *eins = list.head->next;
//        Node *fuenf = eins->next;
//
//        eins->next = list.getMarkedPtr(fuenf);
//
//        Node *left;
//        Node *bla = list.search(10, &left);

//        if (list.getMarkedPtr(eins) > list.head + 1000000)
//            std::cout << "bigger" << std::endl;
//        else
//            std::cout << "smaller" << std::endl;

//        std::cout << "1" << std::endl;
//
//        list.insert(10, -2);
//        std::cout<< (list.del(1,-1)? "deleted": "not deleted")<< std::endl;
        list.print();
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
    unsigned long long insertTicks = 0;
    long long tsxCount = 0;

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
        tsxCount += list.tsxInsertTimeCount[i];
        insertTicks += list.insertTicks[i];

    }


    double a = static_cast<double>(tsxInsertTriesUntilSuccess) / static_cast<double> (absoluteSuccessfulInsertsByTSX);
    double b = static_cast<double>(tsxDeleteTriesUntilSuccess) / static_cast<double> (absoluteSuccessfulDeletesByTSX);

    double tsxTriesPerInsert = static_cast<double>(absoluteInsertTsxTries) / static_cast<double> (pushCount);
    double tsxTriesPerDelete = static_cast<double>(absoluteDeleteTsxTries) / static_cast<double> (delCount);

    double tsxAbortsPerInsert = static_cast<double> (absoluteAbortedInsertTries) / static_cast<double> (pushCount);
    double tsxAbortsPerDelete = static_cast<double> (absoluteAbortedDeleteTries) / static_cast<double> (delCount);

    double ticksPerTry = static_cast<double> (insertTicks) / static_cast<double> (tsxCount);

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
    std::cout << std::endl << "avg ticks per insert transactions: " << ticksPerTry << std::endl;



    std::cout << std::endl << "Numbers are ascending: " << (list.isIncreasing() ? "true" : "false") << std::endl;

    return 0;
}

void doStandardTest(List &list, int numberElements, int numThreads) {
    std::cout << "test start" << std::endl;
    useSome = true;
    upperLimit = 255;
//    upperLimit = ceil(static_cast<double > (numberElements) / 4);
    std::vector<std::thread> tv;
    for (int i = 0; i < numThreads; i++) {
        tv.push_back(std::thread(pushWorker, std::ref(list), numberElements, i));
        tv.push_back(std::thread(deleteWorker, std::ref(list), numberElements, i));
    }
    for (auto &t: tv)
        t.join();
    std::cout << "test end" << std::endl;
}

void prepareListForTest(List &list, int threadCount) {
    list.insertsByNonBlock = new int[threadCount];
    list.insertsByTSX = new int[threadCount];
    list.deletesByNonBlock = new int[threadCount];
    list.deletesByTSX = new int[threadCount];
    list.triesForSuccessfulInsertTSX = new long long[threadCount];
    list.triesForSuccessfulDeleteTSX = new long long[threadCount];
    list.absoluteInsertTsxTries = new long long[threadCount];
    list.absoluteDeleteTsxTries = new long long[threadCount];
    list.abortedTsxInsertTry = new long long[threadCount];
    list.abortedTsxDeleteTry = new long long[threadCount];
    list.insertTicks = new unsigned long long int [threadCount];
    list.tsxInsertTimeCount = new long long[threadCount];

    /// initialize resultChecking arrays
    pushCountArray = new int[threadCount];
    deleteCountArray = new int[threadCount];
    for (int j = 0; j < threadCount; j++) {
        pushCountArray[j] = 0;
        deleteCountArray[j] = 0;
        list.insertsByNonBlock[threadCount] = 0;
        list.insertsByTSX[threadCount] = 0;
        list.deletesByNonBlock[threadCount] = 0;
        list.deletesByTSX[threadCount] = 0;
        list.triesForSuccessfulInsertTSX[num_threads] = 0;
        list.triesForSuccessfulDeleteTSX[num_threads] = 0;
        list.absoluteInsertTsxTries[threadCount] = 0;
        list.absoluteDeleteTsxTries[threadCount] = 0;
        list.abortedTsxInsertTry[threadCount] = 0;
        list.abortedTsxDeleteTry[threadCount] = 0;
        list.insertTicks[j] = 0;
        list.tsxInsertTimeCount[j] = 0;
    }
}

//void test(benchmark::State &state) {
//    std::cout << "+" << std::endl;
//    List list;
//    prepareListForTest(list, 5);
////    for (auto _ : state) {
//    doStandardTest(list, 1000, 5);
//    std::cout << "-" << std::endl;
////    }
////    while (state.KeepRunning()) {
////        std::cout << "starting benchmark | threads: " << state.range(0) << " | numElements: " << state.range(1)
////                  << std::endl;
////        std::cout << "printing list" << std::endl;
////        list.print();
////    }
//
//}
//
//BENCHMARK(test)
////->Args({1, 1000})
////->Args({10, 0})
////->Args({3, 1000})
////->Args({4, 1000})
////        ->Args({6, 0})
////->Args({6, 1000})
////->Args({7, 1000})
////->Args({8, 1000})
////->Args({9, 1000})
////->Args({10, 1000})
//;
//
//BENCHMARK_MAIN();