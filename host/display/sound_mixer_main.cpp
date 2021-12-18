#include <iostream>
#include <thread>
#include <chrono>

#include "sound_mixer.h"
#include "sine_layer.h"
#include "sine_envelope_layer.h"
#include "pwm_layer.h"
#include "balance_layer.h"

int main(int argc, char**argv) {
  airball::sound_mixer m(4);

  airball::sine_layer lo(/* 30 */ 100);
  airball::sine_layer hi(102);

  airball::balance_layer left(20000, 1.0, 0.25);
  airball::balance_layer right(20000, 0.25, 1.0);

  m.set_layer(1, new airball::sine_envelope_layer(44100 * 3));
  m.set_layer(2, new airball::pwm_layer(4000, 2000, 50));

  if (!m.start()) {
    std::cout << "Error" << std::endl;
    return -1;
  }

  bool k = false;

  for (int i = 0; i < 50; i++) {
    m.set_layer(0, k ? &hi : &lo);
    m.set_layer(3, k ? &right : &left);
    std::cout << "did set_layer() " << k << std::endl;
    k = !k;
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(500));
  }

  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1000));

  return 0;
}