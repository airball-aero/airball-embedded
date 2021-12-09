#include "sound_layer.h"

namespace airball {

sound_layer::sound_layer(size_t period)
    : period_(period) {}

size_t sound_layer::period() const {
  return period_;
}

} // namespace airball

