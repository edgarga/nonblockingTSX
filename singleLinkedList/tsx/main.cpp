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

int main(int numberOfArguments, char *arguments[]) {
    mcp_init(numberOfArguments, arguments);

    upperLimit = num_elements;

    /// initialize resultChecking arrays
    pushCountArray = new int[num_threads];
    deleteCountArray = new int[num_threads];
    for (int j = 0; j < num_threads; j++) {
        pushCountArray[j] = 0;
        deleteCountArray[j] = 0;
    }

    /// start the test
    List list;
    list.insertsByNonBlock = new int[num_threads];
    list.insertsByTSX = new int[num_threads];
    list.deletesByNonBlock = new int[num_threads];
    list.deletesByTSX = new int[num_threads];
    list.absoluteTriesInsertTSX = new long long[num_threads];
    list.absoluteTriesDeleteTSX = new long long[num_threads];


    int pushCount = 0;
    int delCount = 0;

    time_start();

    if (nonBlock) {
        list.absoluteTries_Delete = 0;
        list.absoluteTries_Insert = 0;
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
    std::cout << std::endl << std::endl;
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
    long long absoluteTSXInsertTries = 0, absoluteTSXDeleteTries = 0;

    for (int i = 0; i < num_threads; i++) {
        absoluteSuccessfulInsertsByTSX += list.insertsByTSX[i];
        absoluteSuccessfulDeletesByTSX += list.deletesByTSX[i];

        absoluteSuccessfulInsertsByNonBlock += list.insertsByNonBlock[i];
        absoluteSuccessfulDeletesByNonBlock += list.deletesByNonBlock[i];

        absoluteTSXInsertTries += list.absoluteTriesInsertTSX[i];
        absoluteTSXDeleteTries += list.absoluteTriesDeleteTSX[i];
    }


    double a = static_cast<double>(absoluteTSXInsertTries) / static_cast<double> (absoluteSuccessfulInsertsByTSX);
    double b = static_cast<double>(absoluteTSXDeleteTries) / static_cast<double> (absoluteSuccessfulDeletesByTSX);

    std::cout << "nonBlock inserts: " << absoluteSuccessfulInsertsByNonBlock << std::endl;
    std::cout << "tsx inserts: " << absoluteSuccessfulInsertsByTSX << std::endl;
    std::cout << "tsx insert tries absolute: " << absoluteTSXInsertTries << std::endl;
    std::cout << "tries per successful tsx insert: " << a << std::endl;
    std::cout << std::endl;
    std::cout << "nonBlock deletes: " << absoluteSuccessfulDeletesByNonBlock << std::endl;
    std::cout << "tsx deletes: " << absoluteSuccessfulDeletesByTSX << std::endl;
    std::cout << "tsx delete tries absolute: " << absoluteTSXDeleteTries << std::endl;
    std::cout << "tries per successful tsx delete: " << b << std::endl;

    return 0;
}

