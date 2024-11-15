cmake_minimum_required(VERSION 3.28)

add_library(prefix_non_simd STATIC bin.cpp)

target_include_directories(prefix_non_simd PUBLIC ${PROJECT_SOURCE_DIR}/include)