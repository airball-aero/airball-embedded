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

add_library(airball_core
        widgets.cpp airdata.cpp settings.cpp display.cpp one_shot_timer.cpp
        controller.cpp screen.cpp user_input_source.cpp delay_timer.cpp
        aerodynamics.cpp fake_telemetry_client.cpp interpolation_table.cpp
        data_logger.cpp system_status.cpp)

add_library(user_input_source
        user_input_source.cpp)

target_link_libraries(airball_core X11 cairo pthread)

add_executable(ab ab_main.cpp)
target_link_libraries(ab airball_core)

add_executable(ab_device ab_device_main.cpp)
target_link_libraries(ab_device airball_core)

add_executable(user_input_source_test user_input_source_test_main.cpp)
target_link_libraries(user_input_source_test user_input_source)

enable_testing()
add_executable(runUnitTests
        aerodynamics_test.cpp one_shot_timer_test.cpp delay_timer_test.cpp
        interpolation_table_test.cpp airdata_test.cpp
        fake_telemetry_client_test.cpp system_status_test.cpp)
target_link_libraries(runUnitTests gtest gtest_main airball_core)
