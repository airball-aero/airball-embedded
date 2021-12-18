# The MIT License (MIT)
#
# Copyright (c) 2017-2018, Ihab A.B. Awad
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

find_package(Boost REQUIRED program_options iostreams system)
include_directories(${BOOST_INCLUDE_DIRS})
message("boost lib: ${Boost_LIBRARIES}")
message("boost inc: ${Boost_INCLUDE_DIR}")

add_library(airball_display
        widgets.cpp airdata.cpp settings.cpp display.cpp one_shot_timer.cpp
        controller.cpp screen.cpp user_input_source.cpp delay_timer.cpp
        aerodynamics.cpp fake_telemetry_client.cpp interpolation_table.cpp
        system_status.cpp device_keyboard_listener.cpp
        log_reader_telemetry_client.cpp image_screen.cpp climb_rate_filter.cpp
        http_server.cpp file_write_watch.cpp sound_mixer.cpp sound_layer.cpp
		sine_layer.cpp sine_envelope_layer.cpp pwm_layer.cpp balance_layer.cpp)
target_link_libraries(airball_display
        X11 cairo pthread asound
        airball_telemetry_client
        ${Boost_LIBRARIES})

add_executable(sound_mixer
        sound_mixer_main.cpp)
target_link_libraries(sound_mixer
        airball_display)

add_executable(ab
        ab_main.cpp)
target_link_libraries(ab
        airball_display)

add_executable(ab_device
        ab_device_main.cpp)
target_link_libraries(ab_device
        airball_display airball_telemetry_client)

add_executable(replay
        replay_main.cpp)
target_link_libraries(replay
        airball_display airball_telemetry_client)

add_executable(user_input_source_test
        user_input_source_test_main.cpp)
target_link_libraries(user_input_source_test
        airball_display)

add_executable(device_keyboard_listener_test
        device_keyboard_listener_test_main.cpp)
target_link_libraries(device_keyboard_listener_test
        airball_display)

add_executable(http_server_test
        http_server_test_main.cpp)
target_link_libraries(http_server_test
        airball_display)

add_executable(file_write_watch_test
        file_write_watch_test_main.cpp)
target_link_libraries(file_write_watch_test
        airball_display)

enable_testing()
add_executable(display_tests
        aerodynamics_test.cpp one_shot_timer_test.cpp delay_timer_test.cpp
        interpolation_table_test.cpp airdata_test.cpp
        fake_telemetry_client_test.cpp system_status_test.cpp)
target_link_libraries(display_tests
        gtest gtest_main airball_display)
