cmake_minimum_required(VERSION 3.16)
project(core C)

set(CMAKE_C_STANDARD 99)

add_executable(core main.c)
target_link_libraries(core pthread)

