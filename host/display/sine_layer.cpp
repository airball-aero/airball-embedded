#include <cmath>
#include <iostream>

#include "sine_layer.h"

namespace  airball {

sine_layer::sine_layer(snd_pcm_uframes_t period)
    : sound_layer(period), table_(new int16_t[period]) {
  int16_t* t = table_.get();
  for (size_t k = 0; k < period; k++) {
    double ratio = sin(((double) k / (double) period) * M_PI * 2.0);
    *t++ = (int16_t) (ratio * INT16_MAX);
  }
}

void sine_layer::apply(int16_t* buf, snd_pcm_uframes_t frames, snd_pcm_uframes_t pos) const {
  const int16_t* t = table_.get();
  for (size_t i = 0; i < frames; i++) {
    pos %= period();
    *buf++ = t[pos]; // left
    *buf++ = t[pos]; // right
    pos++;
  }
}

} // namespace airball
