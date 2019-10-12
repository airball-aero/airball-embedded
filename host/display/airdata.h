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

#ifndef AIRBALL_AIRDATA_H
#define AIRBALL_AIRDATA_H

#include <string>
#include "interpolation_table.h"
#include "../telemetry/airdata_sample.h"

namespace airball {

class Airdata {
public:
  class Ball {
  public:
    Ball(double alpha, double beta, double ias, double tas)
        : alpha_(alpha), beta_(beta), ias_(ias), tas_(tas) {}
    Ball() : Ball(0, 0, 0, 0) {}
    ~Ball() = default;

    double alpha() const { return alpha_; }
    double beta() const { return beta_; }
    double ias() const { return ias_; }
    double tas() const { return tas_; }

  private:
    double alpha_;
    double beta_;
    double ias_;
    double tas_;
  };

  Airdata();

  // Indicated air speed
  double ias() const {return ias_;}

  // True air speed
  double tas() const {return tas_;}

  // Angle of attack
  double alpha() const {return alpha_;}

  // Angle of yaw
  double beta() const {return beta_;}

  // Free stream dynamic pressure
  double free_stream_q() const {return free_stream_q_;}

  // Current altitude
  double altitude() const { return altitude_; }

  // Current pressure altitude, without barometric compensation
  double pressure_altitude() const { return pressure_altitude_; }

  // Current climb rate
  double climb_rate() const { return climb_rate_; }

  // Returns true if the data is valid. If not, display a red X indicating system failure.
  bool valid() const {return valid_;}

  // Commands this model to update its contents based on the given data.
  //   d -- airdata sample
  //   qnh -- Current sea level barometric pressure in pascals
  //   ball_smoothing_factor -- Smoothing factor for airball data
  //   vsi_smoothing_factor -- Smoothing factor for VSI data
  void update(
      const airdata_sample* d,
      const double qnh,
      const double ball_smoothing_factor,
      const double vsi_smoothing_factor);

  const Ball& smooth_ball() const { return smooth_ball_; }

  const std::vector<Ball>& raw_balls() const { return raw_balls_; }

private:
  static constexpr int kSamplesPerSecond = 20;
  static constexpr uint kNumBalls = 20;

  InterpolationTable dpr_to_angle;
  double ias_;
  double tas_;
  double alpha_;
  double beta_;
  double free_stream_q_;
  double pressure_altitude_;
  double altitude_;
  double climb_rate_;
  bool climb_rate_first_sample_;
  bool valid_;
  Ball smooth_ball_;
  std::vector<Ball> raw_balls_;
};

} // namespace airball

#endif // AIRBALL_AIRDATA_H
