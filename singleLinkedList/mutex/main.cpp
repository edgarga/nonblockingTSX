//
// Created by edgar on 07.01.18.
//
#include <iostream>
#include <thread>
#include <vector>
#include "list.h"
#include "mcp.h"

int upperLimit;
int *insertCountArray;
int *deleteCountArray;

void insertWorker(List &list, int numberOfInserts, int threadId) {
    for (int i = 0; i < numberOfInserts; i++) {
        int valueToInsert = rand() % upperLimit;
//        std::cout << threadId << " inserting: " << valueToInsert<<  " " << upperLimit << std::endl;
        if (list.insert(valueToInsert)) {


            insertCountArray[threadId]++;
        }
    }
}

void deleteWorker(List &list, int numberOfDeletes, int threadId) {
    for (int i = 0; i < numberOfDeletes; i++) {
        int valueToDelete = rand() % upperLimit;
//        std::cout << threadId << " inserting: " << valueToInsert<<  " " << upperLimit << std::endl;
        if (list.del(valueToDelete)) {


            deleteCountArray[threadId]++;
        }
    }
}


int main(int numberOfArguments, char *args[]) {
    mcp_init(numberOfArguments, args);
    insertCountArray = new int[num_threads];
    deleteCountArray = new int[num_threads];

    upperLimit = num_elements;

    for (int i = 0; i < num_threads; i++) {
        insertCountArray[i] = 0;
        deleteCountArray[i] = 0;
    }

    time_start();

    List list;


    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; i++) {
        threads.push_back(std::thread(insertWorker, std::ref(list), num_elements, i));
        threads.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
    }

    for (auto &t: threads)
        t.join();

    std::cout << "inserting done!" << std::endl;

//    std::vector<std::thread> tv;
//    for (int i = 0; i < num_threads; i++) {
//        tv.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
//    }
//
//    for (auto &t: tv)
//        t.join();

    time_stop();
    time_print();


    int elementCount = 0;
    Node *curr = list.head->next;
    while (curr != list.tail) {
        elementCount++;

        curr = curr->next;
    }

    int elementsInserted = 0;
    int elementsDeleted = 0;
    for (int i = 0; i < num_threads; i++) {
        elementsInserted += insertCountArray[i];
        elementsDeleted += deleteCountArray[i];
    }

    std::cout << "elements in List: " << elementCount << std::endl;
    std::cout << "element pushed by threads - deletes: " << elementsInserted - elementsDeleted << std::endl;
    std::cout << "element pushed by threads: " << elementsInserted << std::endl;
    std::cout << "element deleted by threads: " << elementsDeleted << std::endl;

//    list.print();

    return 0;
}