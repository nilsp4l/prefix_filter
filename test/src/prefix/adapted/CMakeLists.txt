enable_testing()


add_executable(
        adapted_bin_test
        adapted_bin_test.cpp
)

target_link_libraries(
        adapted_bin_test
        prefix_adapted
        GTest::gtest_main
)
target_include_directories(adapted_bin_test PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_test(adapted_bin_test adapted_bin_test)
include(GoogleTest)
gtest_discover_tests(adapted_bin_test)
