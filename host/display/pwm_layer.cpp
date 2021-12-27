#include <algorithm>
#include <mutex>
#include "pwm_layer.h"

namespace airball {

pwm_layer::pwm_layer(
    snd_pcm_uframes_t period,
    snd_pcm_uframes_t on_period,
    snd_pcm_uframes_t fade_period) {
  pos_ = 0;
  params_ = make_adjusted_parameters(period, on_period, fade_period);
  changing_ = false;
}

double pwm_layer::factor(snd_pcm_uframes_t k) const {
  k = k % params_.period;
  if (k < params_.on_period) {
    return 1.0;
  }
  double fade_out_value = 1.0 - (double) (k - params_.on_period) / (double) params_.fade_period;
  double fade_in_value = 1.0 - (double) (params_.period - k) / (double) params_.fade_period;
  return std::max(0.0, std::max(fade_in_value, fade_out_value));
}

void pwm_layer::set_parameters(
    snd_pcm_uframes_t period,
    snd_pcm_uframes_t on_period,
    snd_pcm_uframes_t fade_period) {
  std::lock_guard<std::mutex> lock(mu_);
  new_params_ = make_adjusted_parameters(period, on_period, fade_period);
  changing_ = true;
}

pwm_layer::parameters pwm_layer::make_adjusted_parameters(
    snd_pcm_uframes_t period,
    snd_pcm_uframes_t on_period,
    snd_pcm_uframes_t fade_period) {
  return {
      .period = period,
      .on_period = std::min(on_period, period),
      .fade_period = std::min(
          fade_period,
          std::min(
              (period - on_period) / 2,
              on_period)),
  };
}

void pwm_layer::apply(int16_t* buf, snd_pcm_uframes_t frames) {
  std::lock_guard<std::mutex> lock(mu_);
  for (snd_pcm_uframes_t i = 0; i < frames; i++) {
    if (((pos_ + i) % params_.period) == 0 && changing_) {
      params_ = new_params_;
      changing_ = false;
      pos_ = -i;
    }
    double f = factor(pos_ + i);
    for (int j = 0; j < 2; j++) {
      buf[2 * i + j] = (int16_t) ((double) buf[2 * i + j] * f);
    }
  }
  pos_ = (pos_ + frames) % params_.period;
}

} // namespace airball
