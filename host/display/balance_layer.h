#ifndef AIRBALL_DISPLAY_BALANCE_LAYER_H
#define AIRBALL_DISPLAY_BALANCE_LAYER_H

#include "sound_layer.h"

namespace airball {

class balance_layer : public sound_layer {
public:
  explicit balance_layer(size_t period, double left_gain, double right_gain);
  ~balance_layer() override = default;

  void apply(int16_t* buf, size_t frames, size_t pos) const override;
private:
  const double left_gain_;
  const double right_gain_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_BALANCE_LAYER_H
