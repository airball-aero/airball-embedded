#include <cmath>
#include <iostream>

#include "sine_layer.h"

namespace  airball {

sine_layer::sine_layer(snd_pcm_uframes_t period)
    : sound_layer(period) {}

void sine_layer::apply(int16_t* buf, snd_pcm_uframes_t frames, snd_pcm_uframes_t pos) const {
  for (size_t i = 0; i < frames; i++) {
    pos %= period();
    double ratio = sin(((double) pos / (double) period()) * M_PI * 2.0);
    auto value = (int16_t) (ratio * INT16_MAX);
    *buf++ = value; // left
    *buf++ = value; // right
    pos++;
  }
}

} // namespace airball
