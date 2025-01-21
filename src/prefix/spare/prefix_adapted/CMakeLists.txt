add_library(spare_prefix_adapted SHARED)
target_sources(spare_prefix_adapted PRIVATE bin.cpp)

target_include_directories(spare_prefix_adapted PUBLIC ${PROJECT_SOURCE_DIR}/include)