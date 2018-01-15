cmake_minimum_required(VERSION 3.5)

project(crosstest)

SET(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(xyz xyz.cpp)
