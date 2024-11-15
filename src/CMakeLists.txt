set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_NAME prefix_filter)

add_subdirectory(prefix)

add_executable(${EXECUTABLE_NAME} main.cpp)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC prefix_non_simd)