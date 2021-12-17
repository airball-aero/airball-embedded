#ifndef AIRBALL_DISPLAY_PWM_LAYER_H
#define AIRBALL_DISPLAY_PWM_LAYER_H

#include "sound_layer.h"

namespace airball {

class pwm_layer : public sound_layer {
public:
  explicit pwm_layer(size_t period, size_t on_period, size_t fade_period);
  ~pwm_layer() override = default;

  void apply(int16_t* buf, size_t frames, size_t pos) const override;

private:

  double factor(size_t k) const;

  const size_t on_period_;
  const size_t fade_period_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_PWM_LAYER_H
