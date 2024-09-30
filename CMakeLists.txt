cmake_minimum_required(VERSION 3.28)
project(StegoBMP C)

set(CMAKE_C_STANDARD 11)

include_directories(src/include)

add_executable(StegoBMP
        src/include/types.h
        src/mian.c)
