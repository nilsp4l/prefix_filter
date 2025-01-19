cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

project(prefix_filter_dse)

add_compile_options(-Wall -Wextra -Wconversion -pedantic -Wfatal-errors -fsanitize=leak -march=native -msse4.2 -O3)


add_subdirectory(src)
add_subdirectory(include)

set(BENCHMARK_ENABLE_TESTING OFF)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)


FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG c58e6d0710581e3a08d65c349664128a8d9a2461 #v1.9.1
)

FetchContent_MakeAvailable(googletest googlebenchmark)


include(GoogleTest)

enable_testing()


add_subdirectory(test)


add_subdirectory(benchmark)








