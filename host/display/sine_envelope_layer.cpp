#include <cmath>
#include <iostream>

#include "sine_envelope_layer.h"

namespace  airball {

sine_envelope_layer::sine_envelope_layer(snd_pcm_uframes_t period)
    : sound_layer(period) {}

void sine_envelope_layer::apply(int16_t* buf, snd_pcm_uframes_t frames, snd_pcm_uframes_t pos) const {
  for (size_t i = 0; i < frames; i++) {
    pos %= period();
    double ratio = (1.0 + (double) sin( ((double) pos / (double) period()) * M_PI * 2.0)) / 2.0;
    *buf = (int16_t) (((double) *buf) * ratio); // left
    buf++;
    *buf = (int16_t) (((double) *buf) * ratio); // right
    buf++;
    pos++;
  }
}

} // namespace airball
