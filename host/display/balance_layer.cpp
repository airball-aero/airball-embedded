#include "balance_layer.h"

#include <algorithm>
#include <mutex>

namespace airball {

double valid_gain(double x) {
  return std::max(0.0, std::min(1.0, x));
}

balance_layer::balance_layer(
    double left_gain,
    double right_gain,
    snd_pcm_uframes_t ramp_period) {
  goal_ = {
      valid_gain(left_gain),
      valid_gain(right_gain),
  };
  current_ = goal_;
  set_ramp_period(ramp_period);
}

void balance_layer::set_gains(double left_gain, double right_gain) {
  std::lock_guard<std::mutex> lock(mu_);
  goal_ = {
      valid_gain(left_gain),
      valid_gain(right_gain),
  };
}

void balance_layer::set_ramp_period(double ramp_period) {
  std::lock_guard<std::mutex> lock(mu_);
  ramp_period_ = ramp_period;
  gain_step_per_sample_ = 1.0 / (double) ramp_period_;
}

void balance_layer::step_gains_to_goal() {
  current_ = {
      step_gain_to_goal(current_.left, goal_.left),
      step_gain_to_goal(current_.right, goal_.right),
  };
}

double sign(double x) {
  return x < 0 ? -1.0 : 1.0;
}

double balance_layer::step_gain_to_goal(double current, double goal) {
  if (abs(goal - current) < gain_step_per_sample_) {
    return goal;
  }
  return current + gain_step_per_sample_ * sign(goal - current);
}

int16_t scale_value(int16_t value, double scaling) {
  return (int16_t) ((double) value * scaling);
}

void balance_layer::apply(int16_t* buf, snd_pcm_uframes_t frames) {
  std::lock_guard<std::mutex> lock(mu_);
  for (size_t i = 0; i < frames; i++) {
    step_gains_to_goal();
    *buf = scale_value(*buf, current_.left);
    buf++;
    *buf = scale_value(*buf, current_.right);
    buf++;
  }
}

} // namespace airball

