#include <iostream>
#include <thread>
#include <vector>
#include <math.h>
#include "list.h"
#include "mcp.h"
#include "./../../benchmark/include/benchmark/benchmark.h"

void doStandardTest(List &list, int numberElements, int numThreads) {
    std::cout << "test start" << std::endl;
    std::vector<std::thread> tv;
    for (int i = 0; i < numThreads; i++) {
//        tv.push_back(std::thread(pushWorker, std::ref(list), numberElements, i));
//        tv.push_back(std::thread(deleteWorker, std::ref(list), numberElements, i));
    }
    for (auto &t: tv)
        t.join();
    std::cout << "test end" << std::endl;
}

void test(benchmark::State &state) {
    std::cout << "+" << std::endl;
    List list;
//    for (auto _ : state) {
    doStandardTest(list, 1000, 5);
    std::cout << "-" << std::endl;

}

BENCHMARK(test)
//->Args({1, 1000})
//->Args({10, 0})
//->Args({3, 1000})
//->Args({4, 1000})
//        ->Args({6, 0})
//->Args({6, 1000})
//->Args({7, 1000})
//->Args({8, 1000})
//->Args({9, 1000})
//->Args({10, 1000})
;

BENCHMARK_MAIN();