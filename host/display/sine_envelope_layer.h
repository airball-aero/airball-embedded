#ifndef AIRBALL_DISPLAY_SINE_ENVELOPE_LAYER_H
#define AIRBALL_DISPLAY_SINE_ENVELOPE_LAYER_H

#include <memory>
#include "sound_layer.h"

namespace airball {

// A sine_envelope_layer applies a sinusoidally varying gain envelope from 0
// to 1 to the signal currently in the output buffer. Its period is fixed for
// its entire lifetime.
class sine_envelope_layer : public sound_layer {
public:
  explicit sine_envelope_layer(snd_pcm_uframes_t period);
  ~sine_envelope_layer() override = default;

  snd_pcm_uframes_t period() const;

  void apply(int16_t* buf, snd_pcm_uframes_t frames) override;

private:
  const snd_pcm_uframes_t period_;
  snd_pcm_uframes_t pos_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_SINE_ENVELOPE_LAYER_H
