cmake_minimum_required(VERSION 3.16)
project(BigServer C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS -pthread)
add_executable(BigServer main.c DataStructure/Queue.h DataStructure/Queue.c
        DataStructure/Hash.c DataStructure/Hash.h Database/User.h
        Database/User.c Client/Client.c Client/Client.h packageFrom/communication.h packageFrom/message.h packageFrom/BufQueue.c packageFrom/BufQueue.h Clients.c Clients.h)
