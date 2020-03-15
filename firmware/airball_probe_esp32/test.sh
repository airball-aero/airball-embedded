#!/bin/bash

gcc calibration_surface.cpp calibration_surface_test_main.cpp \
    -o calibration_test
./calibration_test
