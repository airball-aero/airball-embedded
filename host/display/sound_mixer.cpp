#include "sound_mixer.h"

#include <cmath>
#include <iostream>
#include <thread>

namespace airball {

sound_mixer::sound_mixer(int nlayers) :
    layers_(nlayers),
    done_(false),
    handle_(nullptr),
    actual_frames_per_period_(kFramesPerPeriod),
    server_([&]() { loop(); }) {
  for (int i = 0; i < nlayers; i++) {
    layers_[i] = nullptr;
  }
}

void sound_mixer::loop() {
  std::unique_lock<std::mutex> start_lock(mut_);
  start_.wait(start_lock);
  start_lock.unlock();

  snd_pcm_uframes_t pos = 0;
  std::unique_ptr<int16_t> buf(new int16_t[actual_frames_per_period_ * 2]);

  while (true) {
    {
      std::lock_guard<std::mutex> loop_lock(mut_);
      if (done_) {
        break;
      }
      for (auto& layer : layers_) {
        if (layer != nullptr) {
          layer->apply(buf.get(), actual_frames_per_period_, pos);
        }
      }

      pos += actual_frames_per_period_;
      size_t max_period = 0;
      for (auto& layer : layers_) {
        if (layer != nullptr) {
          max_period = std::max(max_period, layer->period());
        }
      }
      pos %= max_period;
    }

    int n = snd_pcm_writei(handle_, buf.get(), actual_frames_per_period_);
    if (n < 0) {
      std:: cout << snd_strerror(n) << " " << std::flush;
    }
  }
}

void sound_mixer::set_layer(int idx, const sound_layer *layer) {
  if (!(idx < layers_.size())) {
    return;
  }
  std::lock_guard<std::mutex> guard(mut_);
  layers_[idx] = layer;
}

bool sound_mixer::start() {
  int rc = 0;

  if ((rc = snd_pcm_open(&handle_, "hw:0", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    std::cout << "snd_pcm_open " << snd_strerror(rc) << std::endl;        
    return false;
  }

  snd_pcm_hw_params_t *params;
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(handle_, params);

  snd_pcm_hw_params_set_access(handle_, params,
                               SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(handle_, params, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_channels(handle_, params, 2);

  auto sample_rate = (unsigned int) kSampleRate;
  snd_pcm_hw_params_set_rate_near(handle_, params, &sample_rate, nullptr);
  snd_pcm_hw_params_set_period_size_near(handle_, params, &actual_frames_per_period_, nullptr);
  snd_pcm_hw_params_set_buffer_size(handle_, params, actual_frames_per_period_ * 3);

  if ((rc = snd_pcm_hw_params(handle_, params)) < 0) {
    std::cout << "snd_pcm_hw_params " << snd_strerror(rc) << std::endl;    
    return false;
  }

  if ((rc = snd_pcm_hw_params_get_period_size(params, &actual_frames_per_period_, nullptr)) < 0) {
    std::cout << "snd_pcm_hw_params_get_period_size " << snd_strerror(rc) << std::endl;
    return false;
  }

  start_.notify_one();

  return true;
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
