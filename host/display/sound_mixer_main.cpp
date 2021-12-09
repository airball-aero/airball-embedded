#include <iostream>

#include "sound_mixer.h"
#include "sine_layer.h"
#include "sine_envelope_layer.h"

int main(int argc, char**argv) {
  airball::sound_mixer m(2);
  m.set_layer(0, new airball::sine_layer(120));
  m.set_layer(1, new airball::sine_envelope_layer(44100));
  if (!m.start()) {
    std::cout << "Error" << std::endl;
  }
  Pa_Sleep( 100 * 1000 );
}
