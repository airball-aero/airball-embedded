#include "sound_mixer.h"

#include <cmath>
#include <iostream>
#include <thread>

namespace airball {

sound_mixer::sound_mixer(std::string device_name)
    : device_name_(device_name),
      done_(false),
      handle_(nullptr),
      actual_rate_(kDesiredRate),
      actual_period_size_(kDesiredPeriodSize),
      server_([&]() { loop(); }) {}

void sound_mixer::loop() {
  std::unique_lock<std::mutex> start_lock(mut_);
  start_.wait(start_lock);
  start_lock.unlock();

  const snd_pcm_uframes_t buffer_size = actual_period_size_ * 2 /* channels */;

  std::unique_ptr<int16_t> buf(new int16_t[buffer_size]);

  while (true) {
    {
      std::lock_guard<std::mutex> loop_lock(mut_);
      if (done_) {
        break;
      }

      memset(buf.get(), 0, sizeof(int16_t) * buffer_size);

      for (auto& layer : layers_) {
        if (layer != nullptr) {
          layer->apply(buf.get(), actual_period_size_);
        }
      }
    }

    int n = snd_pcm_writei(handle_, buf.get(), actual_period_size_);
    if (n < 0) {
      std:: cerr << snd_strerror(n) << " " << std::flush;
    }
  }
}

void sound_mixer::set_layers(std::vector<sound_layer *> layers) {
  std::lock_guard<std::mutex> mod_lock(mut_);
  layers_ = layers;
}

bool sound_mixer::start() {
  int rc = 0;

  if ((rc = snd_pcm_open(&handle_, device_name_.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    std::cerr << "snd_pcm_open " << snd_strerror(rc) << std::endl;
    return false;
  }

  snd_pcm_hw_params_t *params;
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(handle_, params);

  snd_pcm_hw_params_set_access(handle_, params,
                               SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(handle_, params, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_channels(handle_, params, 2);

  snd_pcm_hw_params_set_rate_near(handle_, params, &actual_rate_, nullptr);
  snd_pcm_hw_params_set_period_size_near(handle_, params, &actual_period_size_, nullptr);
  snd_pcm_hw_params_set_buffer_size(handle_, params, actual_period_size_ * 3);

  if ((rc = snd_pcm_hw_params(handle_, params)) < 0) {
    std::cerr << "snd_pcm_hw_params " << snd_strerror(rc) << std::endl;
    return false;
  }

  if ((rc = snd_pcm_hw_params_get_rate(params, &actual_rate_, nullptr)) < 0) {
    std::cerr << "snd_pcm_hw_params_get_rate " << snd_strerror(rc) << std::endl;
    return false;
  }

  if ((rc = snd_pcm_hw_params_get_period_size(params, &actual_period_size_, nullptr)) < 0) {
    std::cerr << "snd_pcm_hw_params_get_period_size " << snd_strerror(rc) << std::endl;
    return false;
  }

  start_.notify_one();

  return true;
}

unsigned int sound_mixer::actual_rate() {
  return actual_rate_;
}

snd_pcm_uframes_t sound_mixer::actual_period_size() {
  return actual_period_size_;
}

snd_pcm_uframes_t sound_mixer::seconds_to_frames(double seconds) {
  return (snd_pcm_uframes_t) (seconds * ((double) actual_rate_));
}

snd_pcm_uframes_t sound_mixer::frequency_to_period(double cycles_per_second) {
  return seconds_to_frames(1.0 / cycles_per_second);
}

sound_mixer::~sound_mixer() {
  {
    std::lock_guard<std::mutex> lock(mut_);
    start_.notify_one();
    done_ = true;
  }
  server_.join();
}

} // namespace airball
