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

double gage_pressure_at_point(
    double dynamic_pressure,
    double angle_from_stagnation_point) {
  return
      dynamic_pressure *
      (1.0 - 9.0 / 4.0 * pow(cos(angle_from_stagnation_point - M_PI / 2.0), 2));
}

struct Model {
  double min;
  double max;
};

constexpr static Model kBaroPressure{
    .min =  20000,
    .max = 101325,
};

constexpr static Model kTemperature{
    .min =   0,
    .max =  20,
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
    .min = knots_to_meters_per_second(100),
    .max = knots_to_meters_per_second(0),
};

constexpr static double kProbeHalfAngle = degrees_to_radians(45);

double interpolate_value(double phase_ratio, const Model& m) {
  double factor = (sin(phase_ratio * 2.0 * M_PI) + 1.0) / 2.0;
  return m.min + factor * (m.max - m.min);
}

double magnitude(double ax, double ay) {
  return sqrt(pow(ax, 2) + pow(ay, 2));
}

std::string make_fake_data_sentence(double phase_ratio) {
  double baro_pressure = interpolate_value(phase_ratio, kBaroPressure);
  double temperature = interpolate_value(phase_ratio, kTemperature);
  double dynamic_pressure = ias_to_q(interpolate_value(phase_ratio, kIas));
  double alpha = interpolate_value(phase_ratio, kAlpha);
  double beta = interpolate_value(phase_ratio, kBeta);

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