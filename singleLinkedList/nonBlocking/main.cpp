#include <iostream>
#include <thread>
#include <vector>
#include "list.h"
#include "mcp.h"

int *pushCountArray, *deleteCountArray;
int upperLimit = 0;
std::atomic<bool> bla(false);
void pushWorker(List &list, int pushCount, int threadId) {

    for (int i = 0; i < pushCount; i++) {
        if (list.insert(rand() % upperLimit)) {
            pushCountArray[threadId]++;

        }
    }
}


void deleteWorker(List &list, int deleteCount, int threadId) {
    for (int i = 0; i < deleteCount; i++) {
        int value = rand() % upperLimit;
        if (list.del(value)){
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

    List list;
    std::vector<std::thread> threadVector;
    time_start();
    for (int i = 0; i < num_threads; i++) {
        threadVector.push_back(std::thread(pushWorker, std::ref(list), num_elements, i));
        threadVector.push_back(std::thread(deleteWorker, std::ref(list), num_elements, i));
    }

    for (auto &t: threadVector)
        t.join();













    time_stop();
    int someCount = 0;
    for (int x = 0; x < num_threads; x++) {
        someCount += pushCountArray[x] - deleteCountArray[x];
    }

    int count = 0;
    Node *curr = list.head;
    while (curr->next != list.tail) {
        if (!curr->markedToDelete)
            count++;
        curr = curr->next;
    }
    std::cout << "count: " << count << " | countByThreads: " << someCount << std::endl;
    time_print();
    return 0;
}

