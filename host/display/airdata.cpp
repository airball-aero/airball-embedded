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
    : alpha_(0),
      beta_(0),
      ias_(0),
      climb_rate_(0),
      climb_rate_first_sample_(true),
      raw_balls_(kNumBalls) {
  populate_table(dpr_to_angle);
}

static double
smooth(double current_value, double new_value, double smoothing_factor) {
  return (smoothing_factor * new_value) +
         ((1.0 - smoothing_factor) * current_value);
}

void Airdata::update(
    const airdata_sample *d,
    const double qnh,
    const double ball_smoothing_factor,
    const double vsi_smoothing_factor) {
  double dp0 = d->get_dp0();
  double dpa = d->get_dpA();
  double dpb = d->get_dpB();

  double alpha = -find_dpr_to_angle(dpr_to_angle, dpa / dp0);
  double beta = find_dpr_to_angle(dpr_to_angle, dpb / dp0);
  const double total_angle = sqrt(alpha_ * alpha_ + beta_ * beta_);
  const double q = dp0 / single_point_sphere_pressure_coefficient(total_angle);

  update(alpha, beta, q, d->get_baro(), d->get_temperature(), qnh,
         ball_smoothing_factor, vsi_smoothing_factor);
}

void Airdata::update(
    const airdata_reduced_sample *d,
    const double qnh,
    const double ball_smoothing_factor,
    const double vsi_smoothing_factor) {
  update(d->get_alpha(), d->get_beta(), d->get_q(), d->get_baro(),
         d->get_temperature(), qnh, ball_smoothing_factor,
         vsi_smoothing_factor);
}

void Airdata::update(
    const double alpha,
    const double beta,
    const double q,
    const double p,
    const double t,
    const double qnh,
    const double ball_smoothing_factor,
    const double vsi_smoothing_factor) {
  free_stream_q_ = q;

  double new_ias = q_to_ias(q);
  double new_tas = q_to_tas(free_stream_q_, p, t);

  // If we allow NaN's to get through, they will "pollute" the smoothing
  // computation and every smoothed value thereafter will be NaN. This guard
  // is therefore necessary prior to using data as a smoothing filter input.
  double new_alpha = isnan(alpha) ? alpha_ : alpha;
  double new_beta = isnan(beta) ? beta_ : beta;
  new_ias = isnan(new_ias) ? ias_ : new_ias;
  new_tas = isnan(new_tas) ? tas_ : new_tas;

  smooth_ball_ = Ball(
      smooth(smooth_ball_.alpha(), alpha, ball_smoothing_factor),
      smooth(smooth_ball_.beta(), beta, ball_smoothing_factor),
      smooth(smooth_ball_.ias(), new_ias, ball_smoothing_factor),
      smooth(smooth_ball_.tas(), new_tas, ball_smoothing_factor));

  alpha_ = new_alpha;
  beta_ = new_beta;
  ias_ = new_ias;
  tas_ = new_tas;

  for (int i = raw_balls_.size() - 1; i > 0; i--) {
    raw_balls_[i] = raw_balls_[i - 1];
  }
  raw_balls_[0] = Ball(alpha_, beta_, ias_, tas_);

  altitude_ = pressure_to_altitude(t, p, qnh);

  double new_pressure_altitude = pressure_to_altitude(t, p, QNH_STANDARD);
  double instantaneous_climb_rate =
      (new_pressure_altitude - pressure_altitude_) * kSamplesPerSecond;
  pressure_altitude_ = new_pressure_altitude;

  climb_rate_ = smooth(
      climb_rate_,
      instantaneous_climb_rate,
      vsi_smoothing_factor);

  if (climb_rate_first_sample_) {
    climb_rate_ = 0.0;
    instantaneous_climb_rate = 0;
    climb_rate_first_sample_ = false;
  }

  valid_ = !isnan(alpha_) && !isnan(beta_);
}

} // namespace airball
