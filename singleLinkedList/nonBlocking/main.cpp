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

int some = 100;
bool useSome = false;

void pushWorker(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        int value = rand() % upperLimit;
        if (useSome)
            value = some;
        if (list.insert(value)) {
            pushCountArray[threadId]++;

        }
    }
}


void deleteWorker(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        int value = rand() % upperLimit;

        if (useSome)
            value = some;
        if (list.del(value)) {
            deleteCountArray[threadId]++;
        }

    }
}

int main(int numberOfArguments, char *arguments[]) {
    mcp_init(numberOfArguments, arguments);

    upperLimit = num_elements;

//    initialize resultChecking arrays
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

//    std::vector<std::thread> threadVector;
//    time_start();
//    for (int i = 0; i < num_threads; i++) {
//        threadVector.push_back(std::thread(pushWorker, std::ref(list), num_elements, i));
////        threadVector.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
//    }
//
//    for (auto &t: threadVector)
//        t.join();
//
//    time_stop();
//
//    /**
//     * calculate the successful insert and delete operations
//     */
//    int pushCount = 0;
//    int delCount = 0;
//    for (int x = 0; x < num_threads; x++) {
//        pushCount += pushCountArray[x];
//        delCount += deleteCountArray[x];
//    }
//
//    /**
//     * traverse the List and count the actual present Nodes
//     */
//    int count = 0;
//    Node *curr = list.head;
//    while (curr->next != list.tail) {
//        if (curr->marked == list.toFalse)
//            count++;
//        curr = curr->next;
//    }
//    std::cout << "count: " << count << " | countByThreads: " << pushCount - delCount << std::endl;
//    std::cout << "inserts: " << pushCount << " | deletes: " << delCount << std::endl;
//    time_print();
//    list.print();




    useSome = true;

//    list.insert(98);
//    list.insert(99);
////    list.insert(100);
//    list.insert(101);
//    list.insert(102);

    int pushCount = 0;
    int delCount = 0;
    time_start();
    for (int j = 0; j < num_elements; j++) {
//        std::cout << "go for: " << some << std::endl;


        if (list.insert(some))
            pushCount++;
//        list.print();
        std::vector<std::thread> tv;
        for (int i = 0; i < num_threads; i++) {
            tv.push_back(std::thread(pushWorker, std::ref(list), num_elements, i));
            tv.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
        }
        for (auto &t: tv)
            t.join();
//        list.numTreads = 0;
//        list.lastThread = -1;
//        list.print();
//        std::cout << "finish for: " << some << std::endl;
        some++;
//        if(list.toFalse == list.toTrue)
//            std::cout << "very bad!########################" <<  " true: " << *list.toTrue << "; false: " << *list.toFalse << std::endl;
    }


//    for(int i= 0; i<100; i++){
//        list.insert(i);
//    }
//    list.del(0,10);
//
//    Node* x= list.head;
//    while (x != list.tail){
//        if(x->marked != list.toTrue && x->marked != list.toFalse)
//        {
//            std::cout << "asdfgh " << x->key << std::endl;
//
//        }
//        x = x->next;
//    }

    time_stop();

    /**
     * calculate the successful insert and delete operations
     */
    for (int x = 0; x < num_threads; x++) {
        pushCount += pushCountArray[x];
        delCount += deleteCountArray[x];
    }

    std::cout << "push - del calc" << std::endl;

    /**
     * traverse the List and count the actual present Nodes
     */
    int count = 0;
    Node *curr = list.head;
    while (curr->next != list.tail) {
        if (curr->marked == list.toFalse)
            count++;
        curr = curr->next;
    }
    std::cout << "count: " << count << " | countByThreads: " << pushCount - delCount << std::endl;
    std::cout << "inserts: " << pushCount << " | deletes: " << delCount << std::endl;
//    time_print();


//   list.insert(1);
//
//
//    list.print();


    return 0;
}

