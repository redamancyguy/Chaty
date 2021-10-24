cmake_minimum_required(VERSION 3.16)
project(learning C)

set(CMAKE_C_STANDARD 11)

add_executable(learning amil2.c processPrivileges.cpp memory.cpp context.cpp context.h email.cpp main.cpp)
