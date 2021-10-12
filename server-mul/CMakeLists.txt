cmake_minimum_required(VERSION 3.16)
project(server C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS -pthread)
add_executable(server main.c user.c user.h client.c client.h commondata.h messagequeue.c messagequeue.h)
