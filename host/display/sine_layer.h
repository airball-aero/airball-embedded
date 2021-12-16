#ifndef AIRBALL_DISPLAY_SINE_LAYER_H
#define AIRBALL_DISPLAY_SINE_LAYER_H

#include <memory>
#include "sound_layer.h"

namespace airball {

class sine_layer : public sound_layer {
public:
  explicit sine_layer(size_t period);
  ~sine_layer() override = default;

  void apply(float* buf, size_t frames, size_t pos) const override;

private:
  const std::unique_ptr<float> table_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_SINE_LAYER_H
