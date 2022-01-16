#include <cmath>
#include <iostream>

#include "sine_envelope_layer.h"

namespace  airball {

sine_envelope_layer::sine_envelope_layer(snd_pcm_uframes_t period)
  : period_(std::max((snd_pcm_uframes_t) 1, period)), pos_(0) {}

snd_pcm_uframes_t sine_envelope_layer::period() const {
  return period_;
}

void sine_envelope_layer::apply(int16_t* buf, snd_pcm_uframes_t frames) {
  for (size_t i = 0; i < frames; i++) {
    double ratio = (1.0 + (double) sin( ((double) (pos_ % period_) / (double) period()) * M_PI * 2.0)) / 2.0;
    auto value = (int16_t) (((double) *buf) * ratio); // left
    *buf = *(buf + 1) = value;
    buf += 2;
    pos_++;
  }
  pos_ %= period_;
}

} // namespace airball
