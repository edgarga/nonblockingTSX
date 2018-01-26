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

void pushWorkerIntense(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        int value = rand() % 10;
        if (list.insert(value)) {
            pushCountArray[threadId]++;

        }
    }
}


void deleteWorkerIntense(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        int value = rand() % 10;
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
    if (standardTest)
        for (int j = 0; j < num_elements; j++) {
//        std::cout << "go for: " << some << std::endl;


            if (list.insert(some))
                pushCount++;
//        list.print();
            std::vector<std::thread> tv;
            for (int i = 0; i < num_threads; i++) {
                tv.push_back(std::thread(pushWorker, std::ref(list), 10, i));
                tv.push_back(std::thread(deleteWorker, std::ref(list), 10, i));
            }
            for (auto &t: tv)
                t.join();

//	if(list.del(some))
//		delCount++;

//        list.numTreads = 0;
//        list.lastThread = -1;
//        list.print();
//        std::cout << "finish for: " << some << std::endl;
            some++;
//        if(list.toFalse == list.toTrue)
//            std::cout << "very bad!########################" <<  " true: " << *list.toTrue << "; false: " << *list.toFalse << std::endl;
        }

    if (doShort) {
        std::cout << "head " << list.head << std::endl;
        std::vector<std::thread> tv;
        for (int i = 0; i < num_threads; i++) {
            tv.push_back(std::thread(pushWorkerIntense, std::ref(list), num_elements, i));
            tv.push_back(std::thread(deleteWorkerIntense, std::ref(list), num_elements, i));
        }
        for (auto &t: tv)
            t.join();
        list.print();
    }
    if (workbench) {
//        list.insert(10, 1);
//        list.insert(20, 1);
//        Node *ten = list.head->next;
//        Node *t = ten->next;
//        Node *twen = ten->next;
//        std::cout << "ten " << ten->key << std::endl;
//        std::cout << "t " << t->key << std::endl;
//        std::cout << "twen " << twen->key << std::endl;
//
//        ten->next = list.getMarkedPtr(t);
//        std::cout << "ten " << ten->key << std::endl;
//        std::cout << "t " << t->key << std::endl;
//        std::cout << "twen " << twen->key << std::endl;
//
//        Node *ten2 = list.head->next;
//        Node *t2 = ten2->next;
//        std::cout << "ten2 " << ten->key << std::endl;
//        std::cout << "t2 marked " << (list.isMarkedPtr(t2) ? "true" : "false") << std::endl;
//        std::cout << "t marked " << (list.isMarkedPtr(t) ? "true" : "false") << std::endl;
//        std::cout << "twen marked " << (list.isMarkedPtr(twen) ? "true" : "false") << std::endl;
//        std::cout << "t2 " << list.getUnmarkedPtr(t2)->key << std::endl;


             std::cout << "head: " <<  list.head << std::endl;
        for(int i=0; i<num_elements; i++){
            Node *n = new Node(i);
            if(list.isMarkedPtr(n)){
                std::cout << n << " is marked!!!" << std::endl;
            }
//            size_t t = (size_t) n;
//            size_t t2  = t | 1;
//            size_t t3 = t2 & (std::numeric_limits<std::size_t>::max() -1);
////            std::cout << std::dec << t2  << " " << ((t2 | 1) != 0 ? "true":"false") << std::endl;
////            std::cout << n << " " << list.getMarkedPtr(n) << " " << list.getUnmarkedPtr(list.getMarkedPtr(n)) << " " << (list.isMarkedPtr(list.getUnmarkedPtr(list.getMarkedPtr(n))) ? "is marked": "is not marked")<< std::endl;
////            std::cout << n <<  std::endl;
//            std::cout << (n == list.getUnmarkedPtr(list.getMarkedPtr(n)) ? "": "false") << std::endl;
        }


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
        if (!list.isMarkedPtr(curr->next))
            count++;
        curr = list.getUnmarkedPtr(curr->next);
    }
    std::cout << "count: " << count << " | countByThreads: " << pushCount - delCount << std::endl;
    std::cout << "inserts: " << pushCount << " | deletes: " << delCount << std::endl;
    time_print();


//   list.insert(1);
//
//
//    list.print();


    return 0;
}

