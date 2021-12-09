#include <cmath>
#include <iostream>
#include "sound_mixer.h"

namespace airball {

sound_mixer::sound_mixer(int nlayers)
    : stream_(nullptr), pos_(0), layers_(nlayers) {
  for (int i = 0; i < nlayers; i++) {
    layers_[i] = nullptr;
  }
}

void sound_mixer::set_layer(int idx, const sound_layer* layer) {
  if (!(idx < layers_.size())) {
    return;
  }
  // TODO: synchronize!!
  layers_[idx] = layer;
}

bool sound_mixer::start() {
  if (Pa_Initialize() != paNoError) {
    return false;
  }

  PaStreamParameters output_parameters;

  output_parameters.device = Pa_GetDefaultOutputDevice();
  if (output_parameters.device == paNoDevice) {
    return false;
  }

  output_parameters.channelCount = 2;
  output_parameters.sampleFormat = paFloat32 | paNonInterleaved; 
  output_parameters.suggestedLatency =
    Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
  output_parameters.hostApiSpecificStreamInfo = nullptr;

  PaError err = Pa_OpenStream(
      &stream_,
      nullptr,
      &output_parameters,
      kSampleRate,
      paFramesPerBufferUnspecified,
      paClipOff,
      &sound_mixer::pa_callback,
      this);

  if (err != paNoError) {
    stop();
    return false;
  }

  if (Pa_StartStream(stream_) != paNoError) {
    stop();
    return false;
  }

  return true;
}

void sound_mixer::stop() {
  if (stream_ != nullptr) {
    Pa_CloseStream(stream_);
    stream_ = nullptr;
    pos_ = 0;
  }
}

sound_mixer::~sound_mixer() {
  stop();
}

int sound_mixer::pa_callback(const void *input_buffer,
                             void *output_buffer,
                             unsigned long frames_per_buffer,
                             const PaStreamCallbackTimeInfo* time_info,
                             PaStreamCallbackFlags status_flags,
                             void *user_data) {
  return ((sound_mixer*) user_data)->provide_samples(input_buffer,
                                                     output_buffer,
                                                     frames_per_buffer,
                                                     time_info,
                                                     status_flags);
}

int sound_mixer::provide_samples(const void *input_buffer,
                                 void *output_buffer,
                                 unsigned long frames_per_buffer,
                                 const PaStreamCallbackTimeInfo* time_info,
                                 PaStreamCallbackFlags status_flags) {
  auto buffers = (float**) output_buffer;
  for (auto& layer : layers_) {
    layer->apply(buffers[0], buffers[1], frames_per_buffer, pos_);
  }
  pos_ += frames_per_buffer;
  size_t max_period = 0;
  for (auto& layer : layers_) {
    max_period = std::max(max_period, layer->period());
  }
  pos_ %= max_period;

  return paContinue;
}

} // namespace airball
