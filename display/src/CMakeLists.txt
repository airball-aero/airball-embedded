
add_library(airball_core
        widgets.cpp airdata.cpp settings.cpp display.cpp one_shot_timer.cpp
        controller.cpp data_source.cpp screen.cpp
        user_input_source.cpp delay_timer.cpp aerodynamics.cpp
        fake_data_sentence.cpp interpolation_table.cpp data_logger.cpp)

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

add_executable(airdata_plot airdata_plot_main.cpp)
target_link_libraries(airdata_plot airball_core)

enable_testing()
add_executable(runUnitTests
        aerodynamics_test.cpp one_shot_timer_test.cpp delay_timer_test.cpp
        interpolation_table_test.cpp airdata_test.cpp)
target_link_libraries(runUnitTests gtest gtest_main airball_core)
