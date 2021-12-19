#ifndef AIRBALL_DISPLAY_PWM_LAYER_H
#define AIRBALL_DISPLAY_PWM_LAYER_H

#include "sound_layer.h"

namespace airball {

class pwm_layer : public sound_layer {
public:
  explicit pwm_layer(snd_pcm_uframes_t, snd_pcm_uframes_t on_period, snd_pcm_uframes_t fade_period);
  ~pwm_layer() override = default;

  void apply(int16_t* buf, snd_pcm_uframes_t frames, snd_pcm_uframes_t pos) const override;

private:

  double factor(snd_pcm_uframes_t k) const;

  const snd_pcm_uframes_t on_period_;
  const snd_pcm_uframes_t fade_period_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_PWM_LAYER_H
