#include "aerodynamics.h"

#include <math.h>

namespace airball {

constexpr static double kDensityAirStandard = 1.2;
constexpr static double kDryAirGasConstant = 287.058;

double ias_to_q(double ias) {
  return 0.5 * kDensityAirStandard * pow(ias, 2);
}

double q_to_ias(double q) {
  return sqrt(2.0 * q / kDensityAirStandard);
}

double dry_air_density(double p, double t) {
  return p / (kDryAirGasConstant * t);
}

double q_to_tas(double q, double p, double t) {
  return sqrt(2.0 * q / dry_air_density(p, t));
}

} // namespace airball
