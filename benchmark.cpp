#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>

void oddEvenSortSequential(std::vector<int>& arr);
void oddEvenSortParallel(std::vector<int>& arr, int numThreads);

void generateRandomArray(std::vector<int>& arr, size_t size, int seed) {
    arr.resize(size);
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(1, INT_MAX);
    for (auto& x : arr) x = dist(gen);
}

int main() {
    int seed = 42;
    const int rounds = 5;
    std::vector<size_t> sizes = {10000, 20000, 50000, 100000, 150000, 200000, 400000, 1000000, 2000000, 5000000};
    std::vector<int> threadConfigs = {2, 4, 8};

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "--- ODD-EVEN SORT ---\n";

    for (size_t size : sizes) {
        std::cout << "\n>>> Testing Array Size: " << size << "\n";
        
        std::vector<int> base_data;
        generateRandomArray(base_data, size, seed);

        double totalSeq = 0;
        for (int r = 0; r < rounds; ++r) {
            std::vector<int> data = base_data;
            auto start = std::chrono::high_resolution_clock::now();
            oddEvenSortSequential(data);
            auto end = std::chrono::high_resolution_clock::now();
            totalSeq += std::chrono::duration<double, std::milli>(end - start).count();
        }
        double avgSeq = totalSeq / rounds;
        std::cout << "  Sequential Baseline: " << avgSeq << " ms\n";

        for (int t : threadConfigs) {
            double totalPar = 0;
            for (int r = 0; r < rounds; ++r) {
                std::vector<int> data = base_data;
                auto start = std::chrono::high_resolution_clock::now();
                oddEvenSortParallel(data, t);
                auto end = std::chrono::high_resolution_clock::now();
                totalPar += std::chrono::duration<double, std::milli>(end - start).count();
            }
            double avgPar = totalPar / rounds;
            std::cout << "  Parallel (" << t << " threads): " << avgPar 
                      << " ms | Speedup: " << (avgSeq / avgPar) << "\n";
        }
    }
    return 0;
}