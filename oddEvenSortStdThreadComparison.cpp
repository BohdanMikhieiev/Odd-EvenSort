#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <thread>
#include <omp.h>

void oddEvenSortSequential(std::vector<double>& arr);
void oddEvenSortParallel(std::vector<double>& arr, int numThreads);
void generateRandomArray(std::vector<double>& arr, size_t size, int seed);

void stdWorker(std::vector<double>& arr, size_t start, size_t end, size_t n, bool& changed) {
    for (size_t i = start; i + 1 <= end && i + 1 < n; i += 2) {
        if (arr[i] > arr[i + 1]) {
            std::swap(arr[i], arr[i + 1]);
            changed = true;
        }
    }
}

void oddEvenSortStdThread(std::vector<double>& arr, int numThreads) {
    size_t n = arr.size();
    bool sorted = false;
    while (!sorted) {
        sorted = true;
        std::vector<std::thread> threads;
        std::vector<uint8_t> changes(numThreads, 0);
        size_t chunkSize = (n / 2 / numThreads) * 2;

        for (int t = 0; t < numThreads; ++t) {
            size_t s = t * chunkSize;
            size_t e = (t == numThreads - 1) ? n - 1 : s + chunkSize;
            threads.emplace_back([=, &arr, &changes]() {
                bool localChange = false;
                stdWorker(arr, s, e, n, localChange);
                if (localChange) changes[t] = 1;
            });
        }
        for (auto& th : threads) th.join();
        threads.clear();

        for (int t = 0; t < numThreads; ++t) {
            size_t s = t * chunkSize + 1;
            size_t e = (t == numThreads - 1) ? n - 1 : s + chunkSize;
            threads.emplace_back([=, &arr, &changes]() {
                bool localChange = false;
                stdWorker(arr, s, e, n, localChange);
                if (localChange) changes[t] = 1;
            });
        }
        for (auto& th : threads) th.join();

        for (auto c : changes) if (c) sorted = false;
    }
}

/*int main() {
    std::vector<size_t> testSizes = {100000, 200000};
    int numThreads = 8;
    int seed = 123;

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "=== COMPARISON: OpenMP vs std::thread ===\n";

    for (size_t size : testSizes) {
        std::cout << "\nSize: " << size << "\n";
        std::vector<double> base;
        generateRandomArray(base, size, seed);

        auto data = base;
        auto s1 = std::chrono::high_resolution_clock::now();
        oddEvenSortSequential(data);
        auto e1 = std::chrono::high_resolution_clock::now();
        double tSeq = std::chrono::duration<double, std::milli>(e1 - s1).count();
        std::cout << "Sequential:   " << tSeq << " ms\n";

        data = base;
        auto s2 = std::chrono::high_resolution_clock::now();
        oddEvenSortParallel(data, numThreads);
        auto e2 = std::chrono::high_resolution_clock::now();
        double tOmp = std::chrono::duration<double, std::milli>(e2 - s2).count();
        std::cout << "OpenMP (8t):  " << tOmp << " ms | S: " << (tSeq / tOmp) << "x\n";

        data = base;
        auto s3 = std::chrono::high_resolution_clock::now();
        oddEvenSortStdThread(data, numThreads);
        auto e3 = std::chrono::high_resolution_clock::now();
        double tStd = std::chrono::duration<double, std::milli>(e3 - s3).count();
        std::cout << "std::thread:  " << tStd << " ms | Overhead: " << ((tStd - tOmp)/tOmp * 100) << "%\n";
    }
    return 0;
}*/