#ifndef AIRBALL_DISPLAY_SOUND_MIXER_H
#define AIRBALL_DISPLAY_SOUND_MIXER_H

#include <vector>
#include "portaudio.h"
#include "sound_layer.h"

namespace airball {

class sound_mixer {
public:
  explicit sound_mixer(int nlayers);
  ~sound_mixer();

  void set_layer(int idx, const sound_layer* layer);

  bool start();
  void stop();

  static const int kSampleRate = 44100;
  
private:
  PaStream *stream_;
  size_t pos_;
  std::vector<const sound_layer*> layers_;

  static int pa_callback(const void *input_buffer,
                         void *output_buffer,
                         unsigned long frames_per_buffer,
                         const PaStreamCallbackTimeInfo* time_info,
                         PaStreamCallbackFlags status_flags,
                         void *user_data);

  int provide_samples(const void *input_buffer,
                      void *output_buffer,
                      unsigned long frames_per_buffer,
                      const PaStreamCallbackTimeInfo* time_info,
                      PaStreamCallbackFlags status_flags);
};

} // namespace airball

#endif //AIRBALL_DISPLAY_SOUND_MIXER_H
