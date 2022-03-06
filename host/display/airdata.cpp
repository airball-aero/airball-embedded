#include "airdata.h"

#include "aerodynamics.h"
#include "units.h"

namespace airball {

constexpr double kTableAngleMin = degrees_to_radians(0);
constexpr double kTableAngleMax = degrees_to_radians(40);
constexpr int kTableSteps = 100;

constexpr double kProbeHalfAngle = degrees_to_radians(45);
constexpr double kPiOver2 = M_PI / 2.0;
constexpr double kR = 9.0 / 4.0;

constexpr int kClimbRateFilterSizeMin = 1;
constexpr int kClimbRateFilterSizeMax = 100;

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
    : climb_rate_(0),
      raw_balls_(kNumBalls),
      climb_rate_filter_(1) {
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
  const double total_angle = sqrt(alpha * alpha + beta * beta);
  const double q = dp0 / single_point_sphere_pressure_coefficient(total_angle);

  update(
      alpha,
      beta,
      q,
      d->get_baro(),
      d->get_temperature(),
      qnh,
      ball_smoothing_factor,
      vsi_smoothing_factor);
}

void Airdata::update(
    const airdata_reduced_sample *d,
    const double qnh,
    const double ball_smoothing_factor,
    const double vsi_smoothing_factor) {
  update(
      degrees_to_radians(d->get_alpha()),
      degrees_to_radians(d->get_beta()),
      d->get_q(),
      d->get_baro(),
      d->get_temperature(),
      qnh,
      ball_smoothing_factor,
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
  double new_ias = q_to_ias(q);
  double new_tas = q_to_tas(q, p, t);

  // If we allow NaN's to get through, they will "pollute" the smoothing
  // computation and every smoothed value thereafter will be NaN. This guard
  // is therefore necessary prior to using data as a smoothing filter input.
  double new_alpha = isnan(alpha) ? smooth_ball_.alpha() : alpha;
  double new_beta = isnan(beta) ? smooth_ball_.beta() : beta;
  new_ias = isnan(new_ias) ? smooth_ball_.ias() : new_ias;
  new_tas = isnan(new_tas) ? smooth_ball_.tas() : new_tas;

  smooth_ball_ = Ball(
      smooth(smooth_ball_.alpha(), alpha, ball_smoothing_factor),
      smooth(smooth_ball_.beta(), beta, ball_smoothing_factor),
      smooth(smooth_ball_.ias(), new_ias, ball_smoothing_factor),
      smooth(smooth_ball_.tas(), new_tas, ball_smoothing_factor));

  for (int i = raw_balls_.size() - 1; i > 0; i--) {
    raw_balls_[i] = raw_balls_[i - 1];
  }
  raw_balls_[0] = Ball(new_alpha, new_beta, new_ias, new_tas);

  pressure_altitude_ = pressure_to_altitude(t, p, QNH_STANDARD);

  climb_rate_filter_.set_size(
      kClimbRateFilterSizeMin +
      (int) (vsi_smoothing_factor *
             (kClimbRateFilterSizeMax - kClimbRateFilterSizeMin)));
  climb_rate_filter_.put(pressure_altitude_);
  climb_rate_ = climb_rate_filter_.get_rate() / (1.0 / kSamplesPerSecond);

  altitude_ = pressure_to_altitude(t, p, qnh);

  valid_ = !isnan(alpha) && !isnan(beta);
}

} // namespace airball
