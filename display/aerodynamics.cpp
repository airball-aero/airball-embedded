#include "aerodynamics.h"

#include <math.h>

namespace airball {

constexpr static double kDensityAir = 1.2;

double gage_pressure_at_point(
    double dynamic_pressure,
    double angle_from_stagnation_point) {
  return
      dynamic_pressure *
      (1.0 - 9.0 / 4.0 * pow(cos(angle_from_stagnation_point - M_PI / 2.0), 2));
}

double ias_to_q(double ias) {
  return 0.5 * kDensityAir * pow(ias, 2);
}

double q_to_ias(double q) {
  return sqrt(2.0 * q / kDensityAir);
}

} // namespace airball
