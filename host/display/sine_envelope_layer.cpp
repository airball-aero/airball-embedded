#include <cmath>
#include <iostream>

#include "sine_envelope_layer.h"

namespace  airball {

sine_envelope_layer::sine_envelope_layer(size_t period)
    : sound_layer(period), table_(new double[period]) {
  double* t = table_.get();
  for (size_t k = 0; k < period; k++) {
    *t++ = (1.0f + (float) sin( ((double) k / (double) period) * M_PI * 2.0)) / 2.0f;
  }
}

void sine_envelope_layer::apply(int16_t* buf, size_t frames, size_t pos) const {
  const double* t = table_.get();
  for (size_t i = 0; i < frames; i++) {
    pos %= period();
    *buf++ = (int16_t) (((double) *buf) * t[pos]); // left
    *buf++ = (int16_t) (((double) *buf) * t[pos]); // right
    pos++;
  }
}

} // namespace airball
