cmake_minimum_required(VERSION 3.5)
project(airball_probe_telemetry)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(airball_telemetry_client
        xbee_telemetry_client.cpp xbee.cpp xbee_api_payload.cpp sample.cpp format.cpp
        battery_sample.cpp airdata_sample.cpp xbee_api_frame.cpp)
target_link_libraries(airball_telemetry_client
        Threads::Threads)

add_executable(airball_probe_telemetry
        airball_probe_telemetry.cpp)
target_link_libraries(airball_probe_telemetry
        airball_telemetry_client)

add_executable(send_dummy_data
        send_dummy_data_main.cpp)
target_link_libraries(send_dummy_data
        airball_telemetry_client)

enable_testing()
add_executable(airball_probe_telemetry_tests
        xbee_api_frame_test.cpp xbee_api_frame.cpp)
target_link_libraries(airball_probe_telemetry_tests
        gtest gtest_main)
