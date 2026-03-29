#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

void printStep(const std::vector<int>& arr, const std::string& phase, int iter) {
    std::cout << "Iter " << iter << " | " << phase << ": [ ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "]\n";
}

void traceOddEvenSort(std::vector<int> arr) {
    int n = arr.size();
    bool sorted = false;
    int iteration = 1;

    std::cout << "Initial state: [ ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "]\n" << std::string(40, '-') << "\n";

    while (!sorted) {
        sorted = true;

        for (int i = 0; i + 1 < n; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }
        printStep(arr, "Odd Phase ", iteration);

        for (int i = 1; i + 1 < n; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }
        printStep(arr, "Even Phase", iteration);
        
        std::cout << std::string(40, '-') << "\n";
        iteration++;


        if (iteration > n) break;
    }
    std::cout << "Result: Sorted!\n";
}

int main() {
    std::vector<int> testData = {2, 1, 4, 9, 5, 3, 6, 10};
    traceOddEvenSort(testData);
    return 0;
}