cmake_minimum_required(VERSION 3.16)
project(serverPlus)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)
#add_definitions("-Wall -lpthread -g")
add_executable(serverPlus main.cpp file/FileArray.cpp file/FileArray.h)
