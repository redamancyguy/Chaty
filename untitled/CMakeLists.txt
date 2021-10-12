cmake_minimum_required(VERSION 3.16)
project(untitled C)

set(CMAKE_C_STANDARD 99)
SET(CMAKE_C_FLAGS -pthread)
add_executable(untitled main.c)
