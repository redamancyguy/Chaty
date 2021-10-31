cmake_minimum_required(VERSION 3.16)
project(BigServer C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS -pthread)
add_executable(BigServer main.c DataStructure/Queue.h DataStructure/Queue.c
        DataStructure/Hash.c DataStructure/Hash.h Database/User.h
        Database/User.c DataStructure/Stack.h DataStructure/Stack.c
        DataStructure/ArrayList.c DataStructure/ArrayList.h DataStructure/Stack.c
        DataStructure/Stack.h DataStructure/Tree.c DataStructure/Tree.h relevant.h DataStructure/Array.h
        relevant.c BufferQueue/BufferQueue.c BufferQueue/BufferQueue.h DataStructure/HashList.c DataStructure/HashList.h)
