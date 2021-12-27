#include <iostream>
#include <sstream>
#include <cmath>
#include <thread>
#include "pwm_layer.h"

constexpr int16_t kMax = 1000;
constexpr snd_pcm_uframes_t kFrames = 10;

constexpr int cols = 20;

std::string value_str(int k, int16_t value) {
  std::ostringstream str;
  int col = std::round((double) value / (double) kMax * (double) (cols - 1));
  for (int i = 0; i < cols; i++) {
    if (i == col) {
      str << "*";
    } else if (i == 0 || i == cols - 1) {
      str << "|";
    } else if ((k % 10) == 0) {
      str << "-";
    } else {
      str << " ";
    }
  }
  return str.str();
}

void show_frame(int k, int16_t left, int16_t right) {
  std::cout << value_str(k, left) << "     " << value_str(k, right) << std::endl;
}

int main(int argc, char** argv) {
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << "<period> <on_period> <fade_period>" << std::endl;
  }
  airball::pwm_layer p(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
  int16_t buffer[kFrames * 2];
  int k = 0;
  while (true) {
    for (int i = 0; i < kFrames * 2; i++) {
      buffer[i] = kMax;
    }
    p.apply(buffer, kFrames);
    for (int i = 0; i < kFrames; i++) {
      show_frame(k++, buffer[i * 2], buffer[i * 2 + 1]);
      std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(100));
    }
  }
}