#ifndef AIRBALL_DISPLAY_SINE_LAYER_H
#define AIRBALL_DISPLAY_SINE_LAYER_H

#include <memory>
#include "sound_layer.h"

namespace airball {

class sine_layer : public sound_layer {
public:
  explicit sine_layer(snd_pcm_uframes_t period);
  ~sine_layer() override = default;

  void apply(int16_t* buf, snd_pcm_uframes_t frames, snd_pcm_uframes_t pos) const override;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_SINE_LAYER_H
