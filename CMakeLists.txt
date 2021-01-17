# cmake_minimum_required(VERSION <specify CMake version here>)
project(lox C)

set(CMAKE_C_STANDARD 99)

add_executable(lox main.c common.h chunk.h chunk.c memory.h memory.c debug.c debug.h value.c value.h vm.c vm.h)