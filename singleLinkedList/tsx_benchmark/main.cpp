#include <iostream>
#include <thread>
#include <vector>
#include "list.h"
#include "mcp.h"
#include "/usr/local/include/benchmark/benchmark.h"

std::atomic<int> positionKeyForStackFront;

void pushWorker(List &list, int insertCount, int highestPossibleInsertKey, benchmark::State &state) {
    for (int i = 0; i < insertCount; i++) {
        state.counters["insertTries"]++;
        if (list.insert(rand() % highestPossibleInsertKey, state))
            state.counters["succInserts"]++;
    }

}

void deleteWorker(List &list, int deleteCount, int highestPossibleDeleteKey, benchmark::State &state) {
    double counter = 0;
    for (int i = 0; i < deleteCount; i++) {
        state.counters["deleteTries"]++;
        if (list.del(rand() % highestPossibleDeleteKey, state))
            state.counters["succDeletes"]++;
    }
}

void pushStackFrontWorker(List &list, int pushCount, benchmark::State &state) {
    for (int i = 0; i < pushCount; i++) {
        int pushKey = positionKeyForStackFront;
        if (list.insert(pushKey, state))
            positionKeyForStackFront--;
    }
}

void popStackFrontWorker(List &list, int popCount, benchmark::State &state) {
    for (int i = 0; i < popCount; i++) {
        int popKey = positionKeyForStackFront;
        if (list.insert(popKey, state))
            positionKeyForStackFront++;
    }
}


void doRandomNumberTest(List &list, benchmark::State &state) {
    int numThreads = state.range(0);
    int numberElements = state.range(1);
    std::vector<std::thread> tv;
    for (int i = 0; i < numThreads; i++) {
        tv.push_back(std::thread(pushWorker, std::ref(list), numberElements, state.range(3), std::ref(state)));
        tv.push_back(std::thread(deleteWorker, std::ref(list), numberElements, state.range(3), std::ref(state)));
    }
    for (auto &t: tv)
        t.join();
}

void doStackFrontTest(List &list, benchmark::State &state) {
    int numberOfThreads = state.range(0);
    int numberElements = state.range(1);
    std::vector<std::thread> tv;
    positionKeyForStackFront = numberElements * numberOfThreads;
    for (int i = 0; i < numberOfThreads; i++) {
        tv.push_back(std::thread(pushStackFrontWorker, std::ref(list), numberElements, std::ref(state)));
        tv.push_back(std::thread(popStackFrontWorker, std::ref(list), numberElements, std::ref(state)));
    }
    for (auto &t: tv)
        t.join();
}

void randomNumberTest(benchmark::State &state) {
    state.counters["succInserts"] = 0;
    state.counters["succDeletes"] = 0;
    for (auto _ : state) {
        List list;
        list.absoluteTries_Insert = state.range(2);
        list.absoluteTries_Delete = state.range(2);
        doRandomNumberTest(list, state);
    }
    state.counters["succInserts"] /= state.iterations();
    state.counters["succDeletes"] /= state.iterations();
}

void randomNumberTest_withTsxSearch(benchmark::State &state) {
    state.counters["succInserts" + 1] = 0;
    state.counters["succDeletes"] = 0;
    state.counters["tsxInsertAborts"] = 0;
    state.counters["tsxInsertSuccess"] = 0;
    state.counters["tsxInsertTries"] = 0;

    state.counters["tsxInsertTries"] = 0;
    state.counters["tsxInsertTries"] = 0;
    state.counters["tsxInsertTries"] = 0;

    state.counters["tsxInsertTryTimeAvg"] = 0;
    for (auto _ : state) {

        List list;
        list.useTsxSearch = true;
        list.absoluteTries_Insert = state.range(2);
        list.absoluteTries_Delete = state.range(2);
        doRandomNumberTest(list, state);


    }
    state.counters["succInserts"] /= state.iterations();
    state.counters["succDeletes"] /= state.iterations();
    state.counters["insertTries"] /= state.iterations();
    state.counters["tsxInsertSuccess"] /= state.iterations();
    state.counters["tsxInsertTries"] /= state.iterations();
    state.counters["deleteTries"] /= state.iterations();

    state.counters["tsxIT/it"] = state.counters["tsxInsertTries"] / state.counters["insertTries"];
    state.counters["tsxIT/itSucc"] = state.counters["tsxInsertTries"] / state.counters["succInserts"];
    state.counters["tsxDT/dt"] = state.counters["tsxInsertTries"] / state.counters["insertTries"];
    state.counters["tsxDT/dtSucc"] = state.counters["tsxInsertTries"] / state.counters["succDeletes"];

    state.counters["tsxInsertTryTimeAvg"] /= state.counters["tsxInsertTries"];

}

void stackFront(benchmark::State &state) {
    List list;
    list.absoluteTries_Insert = state.range(2);
    list.absoluteTries_Delete = state.range(2);
    for (auto _ : state) {
        doStackFrontTest(list, state);
    }
}

void stackFront_WithTsxSearch(benchmark::State &state) {

    for (auto _ : state) {
        List list;
        list.useTsxSearch = true;
        list.absoluteTries_Insert = state.range(2);
        list.absoluteTries_Delete = state.range(2);
        doStackFrontTest(list, state);
    }


}


//BENCHMARK(randomNumberTest)
//        ->UseManualTime()
//        ->Unit(benchmark::kMicrosecond)
//        ->Iterations(10)
//        ->Args({1, 10000, 0})
//        ->Args({2, 10000, 0})
//        ->Args({3, 10000, 0})
////        ->Args({4, 1000, 2})
////        ->Args({5, 1000, 2})
////        ->Args({6, 1000, 2})
////        ->Args({7, 1000, 2})
////        ->Args({8, 1000, 2})
////        ->Args({9, 1000, 2})
////        ->Args({10, 1000, 2})
//;
//
BENCHMARK(randomNumberTest_withTsxSearch)
        ->Unit(benchmark::kMicrosecond)
        ->Iterations(10)
        ->Args({4, 10000, 2, 1000})
        ->Args({4, 10000, 2, 2000})
        ->Args({4, 10000, 2, 3000})
        ->Args({4, 10000, 2, 4000})
        ->Args({4, 10000, 2, 5000})
        ->Args({4, 10000, 2, 6000})
        ->Args({4, 10000, 2, 7000})
        ->Args({4, 10000, 2, 8000})
        ->Args({4, 10000, 2, 9000})
        ->Args({4, 10000, 2, 10000})
;
//
//BENCHMARK(stackFront)
//        ->Unit(benchmark::kMicrosecond)
//        ->Iterations(10)
//        ->Args({1, 10000, 2})
//        ->Args({2, 10000, 2})
//        ->Args({3, 10000, 2})
////        ->Args({4, 1000, 2})
////        ->Args({5, 1000, 2})
////        ->Args({6, 1000, 2})
////        ->Args({7, 1000, 2})
////        ->Args({8, 1000, 2})
////        ->Args({9, 1000, 2})
////        ->Args({10, 1000, 2})
//;

//BENCHMARK(stackFront_WithTsxSearch)
//        ->Unit(benchmark::kMicrosecond)
//        ->Iterations(10)
//        ->Args({1, 10000, 2})
//        ->Args({2, 10000, 2})
//        ->Args({3, 10000, 2})
////        ->Args({4, 1000, 2})
////        ->Args({5, 1000, 2})
////        ->Args({6, 1000, 2})
////        ->Args({7, 1000, 2})
////        ->Args({8, 1000, 2})
////        ->Args({9, 1000, 2})
////        ->Args({10, 1000, 2})
//;



BENCHMARK_MAIN();