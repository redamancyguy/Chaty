cmake_minimum_required(VERSION 3.16)
project(client C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS -pthread)
add_executable(client main.c include/Stack.h include/Stack.c include/TreeMap.h include/TreeMap.c)
