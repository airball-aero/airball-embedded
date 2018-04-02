cmake_minimum_required(VERSION 3.5)
project(airball_probe_telemetry)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(airball_probe_telemetry airball_probe_telemetry.cpp sample.cpp sample.h airdata_sample.h sampler.h battery_sample.cpp battery_sample.h airdata_sample.cpp xbee.cpp xbee.h xbee_packet.cpp xbee_packet.h)
target_link_libraries(airball_probe_telemetry Threads::Threads)
