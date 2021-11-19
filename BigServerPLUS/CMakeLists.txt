cmake_minimum_required(VERSION 3.16)
project(BigServerPLUS)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS -pthread)
#add_definitions("-Wall -lpthread -g")
AUX_SOURCE_DIRECTORY(file DIR_SRCS)
add_executable(BigServerPLUS file/FileArray.h main.cpp Communication.h User.cpp User.h
        thread/RWLock.cpp thread/RWLock.h BufferQueue.cpp BufferQueue.h Client.cpp Client.h Group.cpp Group.h)
