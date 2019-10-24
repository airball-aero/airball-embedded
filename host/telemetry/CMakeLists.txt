cmake_minimum_required(VERSION 3.5)
project(telemetry)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED program_options iostreams system)
include_directories(${BOOST_INCLUDE_DIRS})
message("boost lib: ${Boost_LIBRARIES}")
message("boost inc: ${Boost_INCLUDE_DIR}")

add_library(airball_telemetry_client
        xbee_telemetry_client.cpp xbee.cpp xbee_api_payload.cpp sample.cpp format.cpp
        battery_sample.cpp airdata_sample.cpp xbee_api_frame.cpp xbee_known_types.cpp
        xbee_utils.cpp)
target_link_libraries(airball_telemetry_client
        Threads::Threads boost_system)

add_executable(monitor_telemetry
        monitor_telemetry_main.cpp)
target_link_libraries(monitor_telemetry
        airball_telemetry_client)

add_executable(log_telemetry
        log_telemetry_main.cpp)
target_link_libraries(log_telemetry
        airball_telemetry_client)

add_executable(send_dummy_data
        send_dummy_data_main.cpp)
target_link_libraries(send_dummy_data
        airball_telemetry_client)

enable_testing()
add_executable(telemetry_tests
        xbee_api_frame_test.cpp xbee_api_payload_test.cpp)
target_link_libraries(telemetry_tests
        gtest gtest_main airball_telemetry_client)
