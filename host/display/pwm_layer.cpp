#include <algorithm>
#include "pwm_layer.h"

namespace airball {

pwm_layer::pwm_layer(snd_pcm_uframes_t period, snd_pcm_uframes_t on_period, snd_pcm_uframes_t fade_period)
    : period_(period),
      on_period_(std::min(on_period, period)),
      fade_period_(std::min(fade_period, (period - on_period) / 2)),
      pos_(0) {}

snd_pcm_uframes_t pwm_layer::period() const {
  return period_;
}

double pwm_layer::factor(snd_pcm_uframes_t k) const {
  k %= period_;
  if (k < on_period_) {
    // Initial "on" period
    return 1.0;
  }
  if (k < (on_period_ + fade_period_)) {
    // Fade out
    return (double) ((on_period_ + fade_period_) - k) / (double) fade_period_;
  }
  if (k < (period_ - fade_period_)) {
    // Next "off" period
    return 0.0;
  }
  // Fade in
  return (double) (k - (period_ - fade_period_)) / (double) fade_period_;
}

void pwm_layer::apply(int16_t* buf, snd_pcm_uframes_t frames) {
  for (size_t i = 0; i < frames; i++) {
    double f = factor(pos_ % period_);
    for (int j = 0; j < 2; j++) {
      *buf = (int16_t) ((double) *buf * f);
      buf++;
    }
    pos_++;
  }
  pos_ %= period_;
}

} // namespace airball
