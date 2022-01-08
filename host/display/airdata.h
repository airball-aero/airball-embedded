#ifndef AIRBALL_DISPLAY_AIRDATA_H
#define AIRBALL_DISPLAY_AIRDATA_H

#include <string>
#include "interpolation_table.h"
#include "../telemetry/airdata_sample.h"
#include "../telemetry/airdata_reduced_sample.h"
#include "iairdata.h"
#include "cubic_rate_filter.h"

namespace airball {

class Airdata : public IAirdata {
public:
  Airdata();

  double altitude() const override { return altitude_; }
  double climb_rate() const override { return climb_rate_; }
  bool valid() const override { return valid_; }
  const Ball& smooth_ball() const override { return smooth_ball_; }
  const std::vector<Ball>& raw_balls() const override { return raw_balls_; }

  // Commands this model to update its contents based on the given data.
  //   d -- airdata sample
  //   qnh -- Current sea level barometric pressure in pascals
  //   ball_smoothing_factor -- Smoothing factor for airball data
  //   vsi_smoothing_facad.smooth_ball().alphator -- Smoothing factor for VSI data

  void update(
      const airdata_sample* d,
      const double qnh,
      const double ball_smoothing_factor,
      const double vsi_smoothing_factor);

  void update(
      const airdata_reduced_sample* d,
      const double qnh,
      const double ball_smoothing_factor,
      const double vsi_smoothing_factor);

private:
  void update(
      const double alpha,
      const double beta,
      const double q,
      const double p,
      const double t,
      const double qnh,
      const double ball_smoothing_factor,
      const double vsi_smoothing_factor);

  static constexpr int kSamplesPerSecond = 20;
  static constexpr uint kNumBalls = 20;

  InterpolationTable dpr_to_angle;

  bool valid_;

  Ball smooth_ball_;
  std::vector<Ball> raw_balls_;

  cubic_rate_filter climb_rate_filter_;
  double climb_rate_;

  double altitude_;
  double pressure_altitude_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_AIRDATA_H
