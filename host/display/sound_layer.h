#ifndef AIRBALL_DISPLAY_SOUND_LAYER_H
#define AIRBALL_DISPLAY_SOUND_LAYER_H

#include <cstddef>
#include <cstdint>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

namespace airball {

class sound_layer {
public:
  explicit sound_layer(snd_pcm_uframes_t period);
  virtual ~sound_layer() = default;

  snd_pcm_uframes_t period() const;

  virtual void apply(int16_t* buf, snd_pcm_uframes_t frames, snd_pcm_uframes_t pos) const = 0;

private:
  const snd_pcm_uframes_t period_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_SOUND_LAYER_H
