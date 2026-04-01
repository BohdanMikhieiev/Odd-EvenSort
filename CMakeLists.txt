cmake_minimum_required(VERSION 3.30)
project(Sem6CourseWork)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

add_executable(Benchmark
        benchmark.cpp
        sequential.cpp
        parallel.cpp
)

add_executable(SequentialTest
        sequentialTest.cpp
)

# Нова ціль
add_executable(ThreadComparison
        oddEvenSortStdThreadComparison.cpp
        sequential.cpp
        parallel.cpp
        benchmark.cpp
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(Benchmark PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(Benchmark PRIVATE -fopenmp -O3)
    target_link_options(Benchmark PRIVATE -fopenmp)

    target_link_libraries(ThreadComparison PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(ThreadComparison PRIVATE -fopenmp -O3)
    target_link_options(ThreadComparison PRIVATE -fopenmp)
endif()