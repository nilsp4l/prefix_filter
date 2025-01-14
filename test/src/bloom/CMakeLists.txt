enable_testing()


add_executable(
        bloom_test
        bloom_test.cpp
)

target_link_libraries(
        bloom_test
        GTest::gtest_main
)
target_include_directories(bloom_test PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_test(bloom_test bloom_test)
include(GoogleTest)
gtest_discover_tests(bloom_test)