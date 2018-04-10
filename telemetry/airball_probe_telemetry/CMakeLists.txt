cmake_minimum_required(VERSION 3.5)
project(airball_probe_telemetry)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(airball_probe_telemetry
        telemetry_client.h
        airball_probe_telemetry.cpp sample.cpp battery_sample.cpp
        airdata_sample.cpp xbee_packet.cpp)
target_link_libraries(airball_probe_telemetry Threads::Threads)
