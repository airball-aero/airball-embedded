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

#include "airdata.h"

#include <iostream>

#include "aerodynamics.h"
#include "units.h"

namespace airball {

constexpr double kTableAngleMin = degrees_to_radians(0);
constexpr double kTableAngleMax = degrees_to_radians(40);
constexpr int kTableSteps = 100;

constexpr double kProbeHalfAngle = degrees_to_radians(45);
constexpr double kPiOver2 = M_PI / 2.0;
constexpr double kR = 9.0 / 4.0;

double cos_sq(double x) {
  double y = cos(x);
  return y * y;
}

double dpr_for_probe(double a) {
  return
      kR *
      (
          (
              cos_sq(a + kProbeHalfAngle - kPiOver2) -
              cos_sq(a - kProbeHalfAngle - kPiOver2)
          )
          /
          (
              1 - kR * cos_sq(a - kPiOver2)
          )
      );

}

double single_point_sphere_pressure_coefficient(double a) {
  return 1.0 - kR * cos_sq(a - kPiOver2);
}

void populate_table(InterpolationTable& table) {
  for (int i = -1; i <= kTableSteps; i++) {
    double ratio = ((double) i) / ((double) kTableSteps);
    double angle = kTableAngleMin + ratio * (kTableAngleMax - kTableAngleMin);
    table.put(angle, dpr_for_probe(angle));
  }
}

double find_dpr_to_angle(InterpolationTable& table, double dpr) {
  return dpr < 0 ? -table.x(-dpr) : table.x(dpr);
}

Airdata::Airdata() {
  populate_table(dpr_to_angle);
}

void Airdata::update_from_sentence(
    const double baro,
    const double temp,
    const double delta_p_0,
    const double delta_p_alpha,
    const double delta_p_beta) {
  const double alpha_radians =
      find_dpr_to_angle(dpr_to_angle, delta_p_alpha / delta_p_0);
  const double beta_radians =
      find_dpr_to_angle(dpr_to_angle, delta_p_beta / delta_p_0);
  const double total_radians =
      sqrt(alpha_radians * alpha_radians + beta_radians * beta_radians);
  const double free_stream_q = delta_p_0 /
      single_point_sphere_pressure_coefficient(total_radians);
  alpha_ = - radians_to_degrees(alpha_radians);
  beta_ = radians_to_degrees(beta_radians);
  ias_ = meters_per_second_to_knots(q_to_ias(free_stream_q));
  tas_ = meters_per_second_to_knots(q_to_tas(free_stream_q, baro, temp));
  valid_ = !isnan(alpha_) && !isnan(beta_);
}

void Airdata::update_from_sentence(const std::string& sentence) {
  double baro;
  double temp;
  double delta_p_0;
  double delta_p_alpha;
  double delta_p_beta;
  int cnt = sscanf(
      sentence.c_str(),
      "%lf,%lf,%lf,%lf,%lf",
      &baro, &temp, &delta_p_0, &delta_p_alpha, &delta_p_beta);
  if (cnt != 5) {
    valid_ = false;
    return;
  }
  update_from_sentence(baro, temp, delta_p_0, delta_p_alpha, delta_p_beta);
}

} // namespace airball

