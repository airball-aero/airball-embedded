cmake_minimum_required(VERSION 3.5)

project(airball)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# TODO: Implementing the polymorphism of classes like DataSource by putting
# a set of factory functions in one single source file means we always link
# to all libraries regardless of whether we use them. So the "device" binary
# ends up linking X11!! Switch to more verbose but more optimizable format of
# one concrete class per file. Also arrange to make the "device" binary
# statically linked.

add_subdirectory(external/rapidjson)
include_directories(external/rapidjson/include)

add_library(airball_core
        widgets.cpp airdata.cpp settings.cpp display.cpp one_shot_timer.cpp
        controller.cpp controller.h data_source.cpp screen.cpp
        user_input_source.cpp delay_timer.cpp aerodynamics.cpp fake_data_sentence.h fake_data_sentence.cpp)

add_library(user_input_source
        user_input_source.cpp)

target_link_libraries(airball_core X11 cairo pthread)

add_executable(ab ab_main.cpp)
target_link_libraries(ab airball_core)

add_executable(ab_device ab_device_main.cpp)
target_link_libraries(ab_device airball_core)

add_executable(fake_data fake_data_main.cpp)
target_link_libraries(fake_data airball_core)

add_executable(user_input_source_test user_input_source_test_main.cpp)
target_link_libraries(user_input_source_test user_input_source)

add_executable(serial_data_source_test serial_data_source_test_main.cpp)
target_link_libraries(serial_data_source_test airball_core)

enable_testing()
add_subdirectory(external/googletest/googletest)
add_executable(runUnitTests
        aerodynamics_test.cpp one_shot_timer_test.cpp delay_timer_test.cpp)
target_link_libraries(runUnitTests gtest gtest_main airball_core)
