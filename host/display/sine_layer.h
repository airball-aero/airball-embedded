#ifndef AIRBALL_DISPLAY_SINE_LAYER_H
#define AIRBALL_DISPLAY_SINE_LAYER_H

#include <memory>
#include "sound_layer.h"

namespace airball {

// A sine_layer produces a sine waveform, overwriting whatever is currently in
// the output buffer. Its period (inverse of frequency) can be changed at any
// time during its operation; it will finish one cycle at the "old" period, then
// start future cycles at the new period.
class sine_layer : public sound_layer {
public:
  explicit sine_layer(snd_pcm_uframes_t period);
  ~sine_layer() override = default;

  void set_period(snd_pcm_uframes_t period);

  snd_pcm_uframes_t period();

  void apply(int16_t* buf, snd_pcm_uframes_t frames) override;

private:
  void set_frame(int16_t* frame, snd_pcm_uframes_t i) const;

  std::mutex mu_;

  snd_pcm_uframes_t period_;
  snd_pcm_uframes_t pos_;

  bool changing_;
  snd_pcm_uframes_t next_period_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_SINE_LAYER_H
