#include <iostream>
#include <thread>
#include <chrono>

#include "sound_mixer.h"
#include "sine_layer.h"
#include "sine_envelope_layer.h"
#include "pwm_layer.h"
#include "balance_layer.h"

int main(int argc, char**argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <device-name>" << std::endl;
    return -1;
  }

  airball::sound_mixer m(argv[1]);

  if (!m.start()) {
    std::cout << "Error" << std::endl;
    return -1;
  }

  std::cout << "actual_rate = " << m.actual_rate() << std::endl;
  std::cout << "actual_period_size = " << m.actual_period_size() << std::endl;

  airball::sine_layer tone(m.frequency_to_period(432));
  airball::balance_layer bounce(1.0, 1.0);
  airball::sine_envelope_layer wawa(m.seconds_to_frames(3));
  airball::pwm_layer dit(
      m.seconds_to_frames(0.1),
      m.seconds_to_frames(0.05),
      m.seconds_to_frames(0.01));

  std::vector<airball::sound_layer*> layers;
  layers.push_back(&tone);
  layers.push_back(&bounce);
  layers.push_back(&wawa);
  layers.push_back(&dit);

  m.set_layers(layers);

  bool k = false;

  for (int i = 0; i < 50; i++) {
    tone.set_period(k ? m.frequency_to_period(440) : m.frequency_to_period(432));
    std::cout << "did loop at " << k << std::endl;
    k = !k;
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(500));
  }

  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1000));

  return 0;
}
