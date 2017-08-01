#include "aerodynamics.h"

#include <math.h>

namespace airball {

constexpr static double kDensityAir = 1.2;

double ias_to_q(double ias) {
  return 0.5 * kDensityAir * pow(ias, 2);
}

double q_to_ias(double q) {
  return sqrt(2.0 * q / kDensityAir);
}

} // namespace airball
