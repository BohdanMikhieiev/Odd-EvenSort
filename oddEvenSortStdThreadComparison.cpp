#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iomanip>
#include <iostream>

void oddEvenSortSequential(std::vector<double>& arr);
void oddEvenSortParallel(std::vector<double>& arr, int numThreads);
void generateRandomArray(std::vector<double>& arr, size_t size, int seed);

class Barrier {
    std::mutex mtx;
    std::condition_variable cv;
    int count;
    int total;
    int generation = 0;
public:
    Barrier(int n) : count(n), total(n) {}
    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        int gen = generation;
        if (--count == 0) {
            generation++;
            count = total;
            cv.notify_all();
        } else {
            cv.wait(lock, [this, gen] { return gen != generation; });
        }
    }
};

void oddEvenSortStdThread(std::vector<double>& arr, int numThreads) {
    size_t n = arr.size();
    if (n < 2) return;

    std::vector<std::atomic<int>> phase_counters(numThreads);
    for(int i = 0; i < numThreads; ++i) phase_counters[i] = 0;

    std::atomic<bool> global_sorted{false};
    Barrier barrier(numThreads);
    std::vector<std::thread> workers;

    for (int tid = 0; tid < numThreads; ++tid) {
        workers.emplace_back([&, tid]() {
            int current_phase = 1;

            size_t pairCount = n / 2;
            size_t chunkSize = (pairCount + numThreads - 1) / numThreads;
            size_t pairStart = tid * chunkSize;
            size_t pairEnd = std::min(pairStart + chunkSize, pairCount);

            while (!global_sorted.load(std::memory_order_acquire)) {
                bool local_sorted = true;

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

                barrier.wait();
                if (tid == 0) global_sorted.store(true, std::memory_order_release);
                barrier.wait();

                if (!local_sorted) {
                    global_sorted.store(false, std::memory_order_release);
                }
                barrier.wait();
            }
        });
    }

    for (auto& worker : workers) {
        worker.join();
    }
}

int main() {
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
}