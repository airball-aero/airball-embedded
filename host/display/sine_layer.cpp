#include <cmath>
#include <iostream>

#include "sine_layer.h"

namespace  airball {

sine_layer::sine_layer(size_t period)
    : sound_layer(period), table_(new float[period]) {
  float* t = table_.get();
  for (size_t k = 0; k < period; k++) {
    *t++ = (float) sin( ((double) k / (double) period) * M_PI * 2.0);
  }
}

void sine_layer::apply(float* buf, size_t frames, size_t pos) const {
  const float* t = table_.get();
  for (size_t i = 0; i < frames; i++) {
    pos %= period();
    *buf++ = t[pos]; // left
    *buf++ = t[pos]; // right
    pos++;
  }
}

} // namespace airball
