
add_executable(
        prefix_benchmarking
        prefix_benchmarking.cpp
)


target_link_libraries(prefix_benchmarking benchmark::benchmark)

target_include_directories(prefix_benchmarking PUBLIC ${PROJECT_SOURCE_DIR}/include)