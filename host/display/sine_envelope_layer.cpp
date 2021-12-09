#include <cmath>
#include <iostream>

#include "sine_envelope_layer.h"

namespace  airball {

sine_envelope_layer::sine_envelope_layer(size_t period)
    : sound_layer(period), table_(new float[period]) {
  float* t = table_.get();
  for (size_t k = 0; k < period; k++) {
    *t++ = (1.0f + (float) sin( ((double) k / (double) period) * M_PI * 2.0)) / 2.0f;
  }
}

void sine_envelope_layer::apply(float* left, float* right, size_t len, size_t pos) const {
  const float* t = table_.get();
  for (size_t i = 0; i < len; i++) {
    pos %= period();
    *left++ *= *t;
    *right++ *= *t;
    t++;
    pos++;
  }
}

} // namespace airball
