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
  explicit sound_mixer(std::string device_name, unsigned int nlayers);
  ~sound_mixer();

  void set_layer(unsigned int idx, const sound_layer* layer);

  bool start();

  unsigned int actual_rate();

  snd_pcm_uframes_t actual_period_size();

  snd_pcm_uframes_t seconds_to_frames(double seconds);

  snd_pcm_uframes_t frequency_to_period(double cycles_per_second);

private:
  static const unsigned int kDesiredRate = 44100;
  static const snd_pcm_uframes_t kDesiredPeriodSize = 96;

  void loop();

  sound_mixer(const sound_mixer&) = delete;
  sound_mixer& operator=(const sound_mixer&) = delete;

  const std::string device_name_;
  std::mutex mut_;
  std::condition_variable start_;
  bool done_;
  std::vector<const sound_layer*> layers_;
  snd_pcm_t* handle_;
  unsigned int actual_rate_;
  snd_pcm_uframes_t actual_period_size_;
  std::thread server_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_SOUND_MIXER_H
