cmake_minimum_required(VERSION 3.9)
project(airball_probe_telemetry)

set(CMAKE_CXX_STANDARD 14)

add_executable(airball_probe_telemetry airball_probe_telemetry.cpp sample.cpp sample.h airdata_sample.h sampler.h battery_sample.cpp battery_sample.h airdata_sample.cpp xbee.cpp xbee.h xbee_packet.cpp xbee_packet.h)