cmake_minimum_required(VERSION 3.21)
project(hw1 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(hw1
        parser.c
        parser.h
        scratch.cpp)
