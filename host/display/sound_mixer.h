#ifndef AIRBALL_DISPLAY_SOUND_MIXER_H
#define AIRBALL_DISPLAY_SOUND_MIXER_H

#include <vector>
#include <condition_variable>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <memory>
#include <mutex>
#include <thread>

#include "sound_layer.h"

namespace airball {

class sound_mixer {
public:
  explicit sound_mixer(int nlayers);
  ~sound_mixer();

  void set_layer(int idx, const sound_layer* layer);

  bool start();

  static const unsigned int kSampleRate = 44100;
  static const snd_pcm_uframes_t kFramesPerPeriod = 96;

private:
  void loop();

  sound_mixer(const sound_mixer&) = delete;
  sound_mixer& operator=(const sound_mixer&) = delete;

  std::mutex mut_;
  std::condition_variable start_;
  bool done_;
  std::vector<const sound_layer*> layers_;
  snd_pcm_t* handle_;
  snd_pcm_uframes_t actual_frames_per_period_;
  std::thread server_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_SOUND_MIXER_H
