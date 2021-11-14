cmake_minimum_required(VERSION 3.16)
project(utils C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS -pthread)
add_executable(utils main.c DataStructure/HashList.h DataStructure/HashList.c DataStructure/Queue.h DataStructure/Queue.c
        DataStructure/ArrayList.h DataStructure/ArrayList.c DataStructure/Stack.h DataStructure/Stack.c
        DataStructure/Tree.h DataStructure/Tree.c)
