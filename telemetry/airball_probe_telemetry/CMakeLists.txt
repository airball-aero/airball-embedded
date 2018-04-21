cmake_minimum_required(VERSION 3.5)
project(airball_probe_telemetry)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(airball_telemetry_client
        xbee_telemetry_client.cpp xbee.cpp xbee_packet.cpp sample.cpp format.cpp
        battery_sample.cpp airdata_sample.cpp)
target_link_libraries(airball_telemetry_client
        Threads::Threads)

add_executable(airball_probe_telemetry
        airball_probe_telemetry.cpp)
target_link_libraries(airball_probe_telemetry
        airball_telemetry_client)
