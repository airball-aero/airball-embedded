#ifndef AIRBALL_DISPLAY_BALANCE_LAYER_H
#define AIRBALL_DISPLAY_BALANCE_LAYER_H

#include "sound_layer.h"

namespace airball {

class balance_layer : public sound_layer {
public:
  explicit balance_layer(double left_gain, double right_gain);
  ~balance_layer() override = default;

  void apply(int16_t* buf, snd_pcm_uframes_t frames) override;

private:
  const double left_gain_;
  const double right_gain_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_BALANCE_LAYER_H
