set(CMAKE_CXX_STANDARD 20)
add_subdirectory(src)


enable_testing()

add_executable(
        run_tests
        run_tests.cpp
)
target_link_libraries(
        run_tests
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
