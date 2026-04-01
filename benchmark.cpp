#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>

void oddEvenSortSequential(std::vector<double>& arr);
void oddEvenSortParallel(std::vector<double>& arr, int numThreads);

void generateRandomArray(std::vector<double>& arr, size_t size, int seed) {
    arr.resize(size);
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(1.0, 1000000.0);
    for (auto& x : arr) x = dist(gen);
}

void verifyCorrectness(size_t size, int numThreads, int seed) {
    std::cout << "--------------------------------------------------\n";
    std::cout << "Validation for N = " << size << "\n";

    std::vector<double> base;
    generateRandomArray(base, size, seed);

    std::vector<double> seqData = base;
    std::vector<double> parData = base;

    std::cout << " > Sequential Sort: ";
    oddEvenSortSequential(seqData);
    bool seqSorted = std::is_sorted(seqData.begin(), seqData.end());
    std::cout << (seqSorted ? "Sorted" : "Error") << "\n";

    std::cout << " > Parallel Sort (threads: " << numThreads << "): ";
    oddEvenSortParallel(parData, numThreads);
    bool parSorted = std::is_sorted(parData.begin(), parData.end());
    std::cout << (parSorted ? "Sorted" : "Error") << "\n";

    std::cout << " > Comparison\n";
    bool match = std::equal(seqData.begin(), seqData.end(), parData.begin());

    if (match && seqSorted && parSorted) {
        std::cout << "Algorithms are working correctly\n";
    } else {
        std::cout << "Error!\n";
    }
    std::cout << "--------------------------------------------------\n";
}

int main() {
    int seed = 42;
    const int rounds = 5;
    std::vector<size_t> sizes = {10000, 20000, 50000, 100000, 150000, 200000, 400000, 1000000, 2000000, 5000000};
    std::vector<int> threadConfigs = {2, 4, 8};

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "--- ODD-EVEN SORT ---\n";

    verifyCorrectness(10000, 8, seed);

    for (size_t size : sizes) {
        std::cout << "\n>>> Testing Array Size: " << size << "\n";
        
        std::vector<double> base_data;
        generateRandomArray(base_data, size, seed);

        double totalSeq = 0;
        for (int r = 0; r < rounds; ++r) {
            std::vector<double> data = base_data;
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
                std::vector<double> data = base_data;
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