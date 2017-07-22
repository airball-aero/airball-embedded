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

#endif  // AIRBALL_UNITS_H