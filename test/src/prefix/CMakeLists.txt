enable_testing()

add_executable(
        pocket_dictionary_test
        pocket_dictionary_test.cpp
)
target_link_libraries(
        pocket_dictionary_test
        GTest::gtest_main
)
target_include_directories(pocket_dictionary_test PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(
        bin_test
        bin_test.cpp
)
target_link_libraries(
        bin_test
        GTest::gtest_main
)
target_include_directories(bin_test PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(
        prefix_filter_test
        prefix_filter_test.cpp
)
target_link_libraries(
        prefix_filter_test
        prefix_adapted
        spare_prefix_adapted
        GTest::gtest_main
)
target_include_directories(prefix_filter_test PUBLIC ${PROJECT_SOURCE_DIR}/include)


add_test(pocket_dictionary_test pocket_dictionary_test)
add_test(bin_test bin_test)
add_test(prefix_filter_test prefix_filter_test)
include(GoogleTest)
gtest_discover_tests(pocket_dictionary_test)
gtest_discover_tests(bin_test)
gtest_discover_tests(prefix_filter_test)

add_subdirectory(adapted)
add_subdirectory(spare)