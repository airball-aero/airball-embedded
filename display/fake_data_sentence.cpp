#include <backward/strstream>
#include "fake_data_sentence.h"

#include "units.h"
#include "aerodynamics.h"

// All engineering values in this module are in SI units:
//     pressure    -- pascals = newtons per meter squared
//     temperature -- degrees Kelvin
//     length      -- meters
//     speed       -- meters per second
//     angle       -- radians
//     density     -- kilograms per meter cubed

namespace airball {

struct Model {
  double min;
  double max;
};

constexpr static Model kBaroPressure{
    .min =  75000,
    .max = 101325,
};

constexpr static Model kTemperature{
    .min =  273,
    .max =  273 + 20,
};

constexpr static Model kAlpha{
    .min = degrees_to_radians(0),
    .max = degrees_to_radians(15),
};

constexpr static Model kBeta{
    .min = degrees_to_radians(-10),
    .max = degrees_to_radians(10),
};

constexpr static Model kIas{
    .min = knots_to_meters_per_second(120),
    .max = knots_to_meters_per_second(0),
};

constexpr static double kProbeHalfAngle = degrees_to_radians(45);

double interpolate_value(double phase_ratio, const Model *m) {
  double factor = (sin(phase_ratio * 2.0 * M_PI) + 1.0) / 2.0;
  return m->min + factor * (m->max - m->min);
}

double magnitude(double ax, double ay) {
  return sqrt(pow(ax, 2) + pow(ay, 2));
}

std::string make_fake_data_sentence(double phase_ratio) {
  double baro_pressure = interpolate_value(phase_ratio, &kBaroPressure);
  double temperature = interpolate_value(phase_ratio, &kTemperature);
  double dynamic_pressure = ias_to_q(interpolate_value(phase_ratio, &kIas));
  double alpha = interpolate_value(phase_ratio, &kAlpha);
  double beta = interpolate_value(phase_ratio, &kBeta);

  double pressure_center = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha, beta));
  double pressure_top = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha + kProbeHalfAngle, beta));
  double pressure_bottom = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha - kProbeHalfAngle, beta));
  double pressure_left = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha, beta + kProbeHalfAngle));
  double pressure_right = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha, beta - kProbeHalfAngle));

  double delta_p_alpha = pressure_top - pressure_bottom;
  double delta_p_beta = pressure_right - pressure_left;

  std::ostrstream result;
  result << baro_pressure << ","
         << temperature << ","
         << pressure_center << ","
         << delta_p_alpha << ","
         << delta_p_beta << std::ends;
  return result.str();
}

}  // namespace airball