/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018, Ihab A.B. Awad
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef AIRBALL_UNITS_H
#define AIRBALL_UNITS_H

#include <math.h>

constexpr double knots_to_meters_per_second(double v) {
  return v / 1.94384;
}

constexpr double meters_per_second_to_knots(double v) {
  return v * 1.94384;
}

constexpr double radians_to_degrees(double v) {
  return v * 180 / M_PI;
}

constexpr double degrees_to_radians(double v) {
  return v / 180 * M_PI;
}

constexpr double celsius_to_kelvin(double v) {
  return v + 273.15;
}

#endif  // AIRBALL_UNITS_H