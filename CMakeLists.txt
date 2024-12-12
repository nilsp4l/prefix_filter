cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(prefix_filter_dse)

add_compile_options(-Wall -Wextra -Wconversion -pedantic -Wfatal-errors -g --assert -fsanitize=leak -march=native)


add_subdirectory(src)
add_subdirectory(include)

add_subdirectory(test)


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)





