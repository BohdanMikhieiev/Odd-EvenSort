#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

void oddEvenSortSequential(std::vector<int>& arr) {
    size_t n = arr.size();
    bool sorted = false;

    while (!sorted) {
        sorted = true;

        for (size_t i = 0; i + 1 < n; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }

        for (size_t i = 1; i + 1 < n; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }
    }
}
