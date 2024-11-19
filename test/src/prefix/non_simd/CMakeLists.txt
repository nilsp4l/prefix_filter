enable_testing()


add_executable(
        bin_test
        bin_test.cpp
)
target_link_libraries(
        bin_test
        GTest::gtest_main
)
target_include_directories(bin_test PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_test(bin_test bin_test)
include(GoogleTest)
gtest_discover_tests(bin_test)
