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

add_test(pocket_dictionary_test pocket_dictionary_test)
include(GoogleTest)
gtest_discover_tests(pocket_dictionary_test)
