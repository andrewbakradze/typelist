cmake_minimum_required(VERSION 3.16)
project(typelist LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

add_library(typelist_lib INTERFACE)
target_compile_features(typelist_lib INTERFACE cxx_std_20)
target_include_directories(typelist_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(typelist main.cpp)
target_link_libraries(typelist PRIVATE typelist_lib)
