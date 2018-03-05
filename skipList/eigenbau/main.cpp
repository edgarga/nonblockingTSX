#include <iostream>
#include <thread>
#include <vector>
#include "skipList.h"
#include "/usr/local/include/benchmark/benchmark.h"

std::atomic<int> insertCount;
std::atomic<int> removeCount;

void insertWorker(SkipList &list, int numberOfTries, int max, int threadId, benchmark::State &state) {
    for (int i = 0; i < numberOfTries; i++) {
        if (list.insert(rand() % max, state))
            insertCount++;
    }
}

void removeWorker(SkipList &list, int numberOfTries, int max, int threadId, benchmark::State &state) {
    for (int i = 0; i < numberOfTries; i++) {
        if (list.remove(rand() % max, state))
            removeCount++;
    }
}

void insertWorker_nb(SkipList &list, int numberOfTries, int max, int threadId) {
    for (int i = 0; i < numberOfTries; i++) {
        if (list.insert(rand() % max))
            insertCount++;
    }
}

void removeWorker_nb(SkipList &list, int numberOfTries, int max, int threadId) {
    for (int i = 0; i < numberOfTries; i++) {
        if (list.remove(rand() % max))
            removeCount++;
    }
}

void print(SkipList &list) {
    std::cout << std::endl << std::endl;
    Node *currentFront = list.headTop;

    while (currentFront != nullptr) {
        Node *currentNode = currentFront;
        while (currentNode != nullptr) {
            if (currentFront != currentNode) std::cout << " -> ";
            if (list.isMarked(currentNode->next))
                std::cout << "\033[1;31m" << currentNode->value << "." << currentNode->level << "\033[0m";
            else

                std::cout << currentNode->value << "." << currentNode->level;
            currentNode = list.getUnmarked(currentNode->next);
        }
        std::cout << std::endl;
        currentFront = currentFront->down;
    }
}

int main() {
    std::cout << "start" << std::endl;
    SkipList list(5, 1);

    insertCount = 0;
    removeCount = 0;

    int num_elements = 100000;
    int maxListLength = 256;
//    insertWorker(list, num_elements, max, 1);
//    removeWorker(list, num_elements, max, 1);
    std::vector<std::thread> tv;

    for (int i = 0; i < 5; i++) {
        tv.push_back(std::thread(insertWorker_nb, std::ref(list), num_elements, maxListLength, i));
        tv.push_back(std::thread(removeWorker_nb, std::ref(list), num_elements, maxListLength, i));
    }
    for (auto &t: tv)
        t.join();

    int inCount1 = 0;
    Node *cur = list.getUnmarked(list.headRootNode->next);
    while (cur != list.tailRootNode) {
        if (!list.isMarked(cur->next))
            inCount1++;
        else
            std::cout << "marked in list" << std::endl;
        cur = list.getUnmarked(cur->next);
    }

    int oldVal = list.headRootNode->value;
    bool isAsc = true;
    cur = list.getUnmarked(list.headRootNode->next);
    while (cur != list.tailRootNode) {
        if (oldVal >= cur->value) {
            isAsc = false;
            break;
        }
        cur = list.getUnmarked(cur->next);
    }


    std::cout << "in List: " << inCount1 << " | count: " << insertCount - removeCount << std::endl;
    std::cout << "inserts: " << insertCount << " | deletes: " << removeCount << std::endl;
    std::cout << std::endl << "max TSX tries: " << list.maxTsxTries << std::endl;
    std::cout << "TSX_Inserts: " << list.tsxInsertCount << "(" <<
              (((static_cast<double> (list.tsxInsertCount)) / (static_cast<double> (insertCount))) * 100)
              << "%)" " | TSX_Deletes: " << list.tsxRemoveCount << "(" <<
              (((static_cast<double> (list.tsxRemoveCount)) / (static_cast<double> (removeCount))) * 100) << "%)"
              << std::endl;
    std::cout << "List is in ascending Order: " << (isAsc ? "true" : "false") << std::endl;
//    print(list);


    return 0;
}

void testMethod(benchmark::State &state) {


    for (auto _ : state) {
        int maxHeightOfSkipList = state.range(0);
        int numberOfListOperations = state.range(1);
        int maxListLength = state.range(2);
        int numberOfThreadsEach = state.range(3);
        int tsxTries = state.range(4);
        SkipList list(maxHeightOfSkipList, tsxTries);

        std::vector<std::thread> tv;
        for (int i = 0; i < numberOfThreadsEach; i++) {
            tv.push_back(std::thread(insertWorker, std::ref(list), numberOfListOperations, maxListLength, i,
                                     std::ref(state)));
            tv.push_back(std::thread(removeWorker, std::ref(list), numberOfListOperations, maxListLength, i,
                                     std::ref(state)));
        }
        for (auto &t: tv)
            t.join();
    }
}

BENCHMARK(testMethod)
        ->Unit(benchmark::kMicrosecond)
        ->Iterations(10)
        ->Args({2, 10000, 10000, 4, 1}) /// Hoehe < 2 macht keinen Sinn, da =2 eine normale Linked List ist
        ->Args({3, 10000, 10000, 4, 1})
        ->Args({4, 10000, 10000, 4, 1})
        ->Args({5, 10000, 10000, 4, 1})
        ->Args({6, 10000, 10000, 4, 1})
        ->Args({7, 10000, 10000, 4, 1})
        ->Args({8, 10000, 10000, 4, 1})
        ->Args({9, 10000, 10000, 4, 1})
        ->Args({10, 10000, 10000, 4, 1})
        ->Args({11, 10000, 10000, 4, 1})
        ->Args({12, 10000, 10000, 4, 1})
        ->Args({13, 10000, 10000, 4, 1})
        ->Args({14, 10000, 10000, 4, 1})
        ->Args({15, 10000, 10000, 4, 1})
        ->Args({16, 10000, 10000, 4, 1})
        ->Args({17, 10000, 10000, 4, 1})
        ->Args({18, 10000, 10000, 4, 1})
        ->Args({19, 10000, 10000, 4, 1})
        ->Args({20, 10000, 10000, 4, 1});

BENCHMARK(testMethod)
        ->Unit(benchmark::kMicrosecond)
        ->Iterations(10)
        ->Args({10, 10000, 10000, 4, 1})
        ->Args({10, 10000, 10000, 4, 2})
        ->Args({10, 10000, 10000, 4, 3})
        ->Args({10, 10000, 10000, 4, 4})
        ->Args({10, 10000, 10000, 4, 5})
        ->Args({10, 10000, 10000, 4, 6})
;

//BENCHMARK_MAIN();