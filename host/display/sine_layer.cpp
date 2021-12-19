#include <cmath>
#include <iostream>
#include <mutex>

#include "sine_layer.h"

namespace  airball {

sine_layer::sine_layer(snd_pcm_uframes_t period)
    : period_(period),
      pos_(0),
      changing_(false),
      next_period_(0) {}

void sine_layer::set_period(snd_pcm_uframes_t period) {
  std::lock_guard<std::mutex> lock(mut_);
  changing_ = true;
  next_period_ = period;
}

snd_pcm_uframes_t sine_layer::period() const {
  return period_;
}

void sine_layer::set_frame(int16_t* frame, snd_pcm_uframes_t i) const {
  double ratio = sin(((double) (i % period_) / (double) period_) * M_PI * 2.0);
  *frame = *(frame + 1) = (int16_t) (ratio * INT16_MAX);
}

void sine_layer::apply(int16_t* buf, snd_pcm_uframes_t frames) {
  std::lock_guard<std::mutex> lock(mut_);
  for (snd_pcm_uframes_t i = 0; i < frames; i++) {
    if (((i + pos_) % period_) == 0 && changing_) {
      period_ = next_period_;
      next_period_ = 0;
      changing_ = false;
      pos_ = -i;
    }
    set_frame(&buf[2 * i], i + pos_);
  }
  pos_ = (pos_ + frames) % period_;
}

} // namespace airball
