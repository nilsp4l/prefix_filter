enable_testing()

add_executable(
        adapted_pocket_dictionary_test
        pocket_dictionary_test.cpp
)
target_link_libraries(
        adapted_pocket_dictionary_test
        GTest::gtest_main
)
target_include_directories(adapted_pocket_dictionary_test PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_test(adapted_pocket_dictionary_test adapted_pocket_dictionary_test)
include(GoogleTest)
gtest_discover_tests(adapted_pocket_dictionary_test)
