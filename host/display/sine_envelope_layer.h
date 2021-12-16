#ifndef AIRBALL_DISPLAY_SINE_ENVELOPE_LAYER_H
#define AIRBALL_DISPLAY_SINE_ENVELOPE_LAYER_H

#include <memory>
#include "sound_layer.h"

namespace airball {

class sine_envelope_layer : public sound_layer {
public:
  explicit sine_envelope_layer(size_t period);
  ~sine_envelope_layer() override = default;

  void apply(float* buf, size_t frames, size_t pos) const override;

private:
  const std::unique_ptr<float> table_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_SINE_ENVELOPE_LAYER_H
