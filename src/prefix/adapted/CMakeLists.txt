cmake_minimum_required(VERSION 3.25)

add_library(prefix_adapted SHARED)
target_sources(prefix_adapted PRIVATE bin.cpp)

target_include_directories(prefix_adapted PUBLIC ${PROJECT_SOURCE_DIR}/include)