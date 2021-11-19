cmake_minimum_required(VERSION 3.16)
project(BigServer C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS -pthread)
add_executable(BigServer main.c DataStructure/Queue.h DataStructure/Queue.c
        User.h
        User.c DataStructure/Stack.h DataStructure/Stack.c
        DataStructure/ArrayList.c DataStructure/ArrayList.h DataStructure/Stack.c
        DataStructure/Stack.h DataStructure/Tree.c DataStructure/Tree.h DataStructure/Array.h
        BufferQueue.c BufferQueue.h DataStructure/HashList.c
        DataStructure/HashList.h ../utils/FileServer/FileServer.c ../utils/FileServer/FileServer.h
        User.c User.h Communication.h FileData/MemoryToFile.h FileData/MemoryToFile.c Client.cpp Client.h)
