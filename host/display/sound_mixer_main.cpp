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

  airball::sound_mixer m(argv[1], 4);

  if (!m.start()) {
    std::cout << "Error" << std::endl;
    return -1;
  }

  airball::sine_layer lo(m.frequency_to_period(432));
  airball::sine_layer hi(m.frequency_to_period(440));

  airball::balance_layer left(1.0, 0.25);
  airball::balance_layer right(0.25, 1.0);

  m.set_layer(
      1,
      new airball::sine_envelope_layer(
          m.seconds_to_frames(3)));
  m.set_layer(
      2,
      new airball::pwm_layer(
          m.seconds_to_frames(0.1),
          m.seconds_to_frames(0.05),
          m.seconds_to_frames(0.01)));

  std::cout << "actual_rate = " << m.actual_rate() << std::endl;
  std::cout << "actual_period_size = " << m.actual_period_size() << std::endl;

  bool k = false;

  for (int i = 0; i < 50; i++) {
    m.set_layer(0, k ? &hi : &lo);
    // m.set_layer(3, k ? &right : &left);
    std::cout << "did set_layer() " << k << std::endl;
    k = !k;
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(500));
  }

  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1000));

  return 0;
}
