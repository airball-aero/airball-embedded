cmake_minimum_required(VERSION 3.5)

project(airball)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(external/rapidjson)
include_directories(external/rapidjson/include)
add_subdirectory(external/googletest/googletest)

add_subdirectory(src)