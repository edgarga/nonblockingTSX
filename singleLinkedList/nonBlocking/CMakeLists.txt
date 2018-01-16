cmake_minimum_required(VERSION 3.9)
project(nonBlocking)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(nonBlocking
        list.cpp
        list.h
        main.cpp
        node.cpp
        node.h
        mcp.c
        mcp.h)
