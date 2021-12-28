#ifndef AIRBALL_DISPLAY_BALANCE_LAYER_H
#define AIRBALL_DISPLAY_BALANCE_LAYER_H

#include <mutex>

#include "sound_layer.h"

namespace airball {

// A balance_layer applies gains to the signals in the output layer. These gains
// range from 0 to 1, and can be different on the left or right sides.
class balance_layer : public sound_layer {
public:
  explicit balance_layer(double left_gain, double right_gain, snd_pcm_uframes_t ramp_period);
  ~balance_layer() override = default;

  void apply(int16_t* buf, snd_pcm_uframes_t frames) override;

  void set_gains(double left_gain, double right_gain);

  void set_ramp_period(snd_pcm_uframes_t ramp_period);

private:
  struct gains {
    double left;
    double right;
  };

  void step_gains_to_goal();
  double step_gain_to_goal(double current, double goal);

  std::mutex mu_;

  snd_pcm_uframes_t ramp_period_;
  double gain_step_per_sample_;

  gains goal_;
  gains current_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_BALANCE_LAYER_H
