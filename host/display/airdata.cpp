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
#include <cstring>

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

void populate_table(InterpolationTable &table) {
  for (int i = -1; i <= kTableSteps; i++) {
    double ratio = ((double) i) / ((double) kTableSteps);
    double angle = kTableAngleMin + ratio * (kTableAngleMax - kTableAngleMin);
    table.put(angle, dpr_for_probe(angle));
  }
}

double find_dpr_to_angle(InterpolationTable &table, double dpr) {
  return dpr < 0 ? -table.x(-dpr) : table.x(dpr);
}

Airdata::Airdata()
    : is_zeroing_(true),
      zero_points_count_(0),
      dp0_zero_offset_(0),
      dpa_zero_offset_(0),
      dpb_zero_offset_(0),
      climb_rate_(0) {
  populate_table(dpr_to_angle);
}

void Airdata::update(const airdata_sample *d, const double qnh) {
  if (is_zeroing_) {
    dp0_zero_offset_ += d->get_dp0();
    dpa_zero_offset_ += d->get_dpA();
    dpb_zero_offset_ += d->get_dpB();
    zero_points_count_++;
    if (zero_points_count_ >= kZeroOffsetPoints) {
      dp0_zero_offset_ /= kZeroOffsetPoints;
      dpa_zero_offset_ /= kZeroOffsetPoints;
      dpb_zero_offset_ /= kZeroOffsetPoints;
      is_zeroing_ = false;
    }
    return;
  }
  double dp0 = d->get_dp0() - dp0_zero_offset_;
  double dpa = d->get_dpA() - dpa_zero_offset_;
  double dpb = d->get_dpB() - dpb_zero_offset_;
  alpha_ = -find_dpr_to_angle(dpr_to_angle, dpa / dp0);
  beta_ = find_dpr_to_angle(dpr_to_angle, dpb / dp0);
  const double total_angle = sqrt(alpha_ * alpha_ + beta_ * beta_);
  free_stream_q_ = dp0 / single_point_sphere_pressure_coefficient(total_angle);
  ias_ = q_to_ias(free_stream_q_);
  tas_ = q_to_tas(free_stream_q_, d->get_baro(), d->get_temperature());
  double new_altitude = pressure_to_altitude(d->get_baro(), qnh);
  double instantaneous_climb_rate = (new_altitude - altitude_) * kSamplesPerSecond;
  altitude_ = new_altitude;
  climb_rate_ = kClimbRateSmoothingFactor * instantaneous_climb_rate + (1.0 - kClimbRateSmoothingFactor) * climb_rate_;
  valid_ = !isnan(alpha_) && !isnan(beta_);
}

} // namespace airball

