#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <omp.h>

void oddEvenSortParallel(std::vector<int>& arr, int numThreads) {
    size_t n = arr.size();
    bool sorted = false;

    size_t pairCount = n / 2;
    int actualThreads = static_cast<int>(std::min((size_t)numThreads, pairCount));

    const size_t CACHE_LINE_INTS = 16;
    size_t chunkSize = (pairCount + actualThreads - 1) / actualThreads;
    chunkSize = ((chunkSize + CACHE_LINE_INTS - 1) / CACHE_LINE_INTS) * CACHE_LINE_INTS;

    #pragma omp parallel num_threads(actualThreads) shared(sorted)
    {
        int tid   = omp_get_thread_num();

        size_t pairStart = tid * chunkSize;
        size_t pairEnd   = std::min(pairStart + chunkSize, pairCount);

        while (!sorted) {
            bool localSorted = true;

            for (size_t p = pairStart; p < pairEnd; p++) {
                size_t i = p * 2;
                if (i + 1 < n && arr[i] > arr[i + 1]) {
                    std::swap(arr[i], arr[i + 1]);
                    localSorted = false;
                }
            }
            #pragma omp barrier

            for (size_t p = pairStart; p < pairEnd; p++) {
                size_t i = p * 2 + 1;
                if (i + 1 < n && arr[i] > arr[i + 1]) {
                    std::swap(arr[i], arr[i + 1]);
                    localSorted = false;
                }
            }

            #pragma omp single
            { sorted = true; }
            #pragma omp barrier

            if (!localSorted) {
            #pragma omp atomic write
                sorted = false;
            }
            #pragma omp barrier
        }
    }
}

