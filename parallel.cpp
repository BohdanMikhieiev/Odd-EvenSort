#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <omp.h>
#include <vector>
#include <atomic>

void oddEvenSortParallel(std::vector<double>& arr, int numThreads) {
    size_t n = arr.size();
    if (n < 2) return;

    std::vector<std::atomic<int>> phase_counters(numThreads);
    for(int i = 0; i < numThreads; ++i) phase_counters[i] = 0;

    std::atomic<bool> global_sorted{false};

    #pragma omp parallel num_threads(numThreads)
    {
        int tid = omp_get_thread_num();
        int current_phase = 1;
        bool local_sorted = false;

        size_t pairCount = n / 2;
        size_t chunkSize = (pairCount + numThreads - 1) / numThreads;
        size_t pairStart = tid * chunkSize;
        size_t pairEnd = std::min(pairStart + chunkSize, pairCount);

        while (!global_sorted) {
            local_sorted = true;

            if (tid > 0)
                while (phase_counters[tid-1].load(std::memory_order_acquire) < current_phase - 1);
            if (tid < numThreads - 1)
                while (phase_counters[tid+1].load(std::memory_order_acquire) < current_phase - 1);

            for (size_t p = pairStart; p < pairEnd; p++) {
                size_t i = p * 2;
                if (i + 1 < n && arr[i] > arr[i + 1]) {
                    std::swap(arr[i], arr[i + 1]);
                    local_sorted = false;
                }
            }

            phase_counters[tid].store(current_phase, std::memory_order_release);
            current_phase++;

            if (tid > 0)
                while (phase_counters[tid-1].load(std::memory_order_acquire) < current_phase - 1);
            if (tid < numThreads - 1)
                while (phase_counters[tid+1].load(std::memory_order_acquire) < current_phase - 1);

            for (size_t p = pairStart; p < pairEnd; p++) {
                size_t i = p * 2 + 1;
                if (i + 1 < n && arr[i] > arr[i + 1]) {
                    std::swap(arr[i], arr[i + 1]);
                    local_sorted = false;
                }
            }

            phase_counters[tid].store(current_phase, std::memory_order_release);
            current_phase++;

            #pragma omp barrier
            if (tid == 0) global_sorted = true;
            #pragma omp barrier

            if (!local_sorted) {
                global_sorted.store(false, std::memory_order_release);
            }
            #pragma omp barrier
        }
    }
}

