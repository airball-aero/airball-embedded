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
  explicit sound_mixer(std::string device_name);
  ~sound_mixer();

  void set_layers(std::vector<sound_layer*> layers);

  bool start();

  unsigned int actual_rate();

  snd_pcm_uframes_t actual_period_size();

  snd_pcm_uframes_t seconds_to_frames(double seconds);

  snd_pcm_uframes_t frequency_to_period(double cycles_per_second);

  snd_pcm_uframes_t octaves_to_period(double base_cycles_per_second,
                                      double octaves);

private:
  static const unsigned int kDesiredRate = 44100;
  static const snd_pcm_uframes_t kDesiredPeriodSize = 256;

  void loop();

  sound_mixer(const sound_mixer&) = delete;
  sound_mixer& operator=(const sound_mixer&) = delete;

  const std::string device_name_;
  std::vector<sound_layer*> layers_;

  std::mutex mut_;
  std::condition_variable start_;
  bool done_;

  std::thread server_;

  snd_pcm_t* handle_;
  unsigned int actual_rate_;
  snd_pcm_uframes_t actual_period_size_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_SOUND_MIXER_H
