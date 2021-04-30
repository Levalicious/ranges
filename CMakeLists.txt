cmake_minimum_required(VERSION 3.19)
project(ranges C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS " -mavx2")

add_executable(ranges main.c types.h range.h)

target_link_libraries(ranges )