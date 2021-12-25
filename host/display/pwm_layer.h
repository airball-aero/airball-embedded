#ifndef AIRBALL_DISPLAY_PWM_LAYER_H
#define AIRBALL_DISPLAY_PWM_LAYER_H

#include "sound_layer.h"

namespace airball {

// A pwm_layer applies a PWM gain schedule to the signal in the output buffer.
// In simple terms, it is a square wave that is either 0 or 1. In practice, it
// is also possible to specify a lead-in / lead-out period to the square wave,
// making it trapezoidal, which can help keep it from sounding too harsh.
//
// The parameters of a pwm_layer are --
//   period -- The period of the entire oscillation.
//   on_period -- The period during which the square wave is turned on. In other
//       words, the duty cycle is (on_period / period).
//   fade_period -- The number of samples for fade-in and fade-out. These are
//       subtracted from the period during which the square wave is "off". If
//       the client specifies fade_period < ((period - on_period) / 2) then the
//       resulting waveform may have a weird shape. However, in all cases, the
//       waveform will be "on" for all of the on_period.
class pwm_layer : public sound_layer {
public:
  explicit pwm_layer(snd_pcm_uframes_t period,
                     snd_pcm_uframes_t on_period,
                     snd_pcm_uframes_t fade_period);
  ~pwm_layer() override = default;

  void set_parameters(snd_pcm_uframes_t period,
                      snd_pcm_uframes_t on_period,
                      snd_pcm_uframes_t fade_period);

  void apply(int16_t* buf, snd_pcm_uframes_t frames) override;

private:

  struct parameters {
    snd_pcm_uframes_t period;
    snd_pcm_uframes_t on_period;
    snd_pcm_uframes_t fade_period;
  };

  double factor(snd_pcm_uframes_t k) const;

  static parameters make_adjusted_parameters(snd_pcm_uframes_t period,
                                             snd_pcm_uframes_t on_period,
                                             snd_pcm_uframes_t fade_period);

  std::mutex mu_;
  parameters new_params_;
  bool changing_;

  parameters params_;
  snd_pcm_uframes_t  pos_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_PWM_LAYER_H
