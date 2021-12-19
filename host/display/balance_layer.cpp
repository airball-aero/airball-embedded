#include <algorithm>
#include "balance_layer.h"

#include "balance_layer.h"

namespace airball {

double valid_gain(double x) {
  return std::max(0.0, std::min(1.0, x));
}

balance_layer::balance_layer(double left_gain, double right_gain)
    : sound_layer(1),
      left_gain_(valid_gain(left_gain)),
      right_gain_(valid_gain(right_gain)) {}

void balance_layer::apply(int16_t* buf, snd_pcm_uframes_t frames, snd_pcm_uframes_t pos) const {
  for (size_t i = 0; i < frames; i++) {
    *buf = (int16_t) ((double) *buf * left_gain_);
    buf++;
    *buf = (int16_t) ((double) *buf * right_gain_);
    buf++;
    pos++;
  }
}

} // namespace airball

