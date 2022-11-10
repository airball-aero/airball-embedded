#include "st7789vi_frame_writer_gpio.h"
#include "st7789vi_frame_writer_smi.h"

#include <unistd.h>
#include <stdint.h>
#include <iostream>

void color(uint16_t* data, uint16_t color) {
  for(int i = 0; i < 320 * 240; i++) {
    data[i] = color;
  }
}

void alternating_bits(uint16_t* data) {
  for(int i = 0; i < 320 * 240; i++) {
    data[i] = (i % 2 == 0) ? 0xaaaa : 0x5555;
  }
}

int main(int argc, char**argv) {
  airball::st7789vi_frame_writer_smi w;
  w.initialize();
  
  uint16_t data[320 * 240];

  color(data, 0xffff);
  w.write_frame(data, 320 * 240);
  usleep(500 * 1000);

  while (true) {
    color(data, 0xf800);
    w.write_frame(data, 320 * 240);
    std::cout << "increasing" << std::endl;
    for (int i = 0; i <= 255; i++) {
      w.set_brightness(i);
      std::cout << "." << std::flush;      
      usleep(10 * 1000);
    }
    std::cout << std::endl;
    usleep(2000 * 1000);
    color(data, 0x07e0);
    w.write_frame(data, 320 * 240);
    std::cout << "decreasing" << std::endl;    
    for (int i = 255; i >= 0; i--) {
      w.set_brightness(i);
      std::cout << "." << std::flush;
      usleep(10 * 1000);
    }
    std::cout << std::endl;    
    usleep(2000 * 1000);
  }
  
  while (true) {
    alternating_bits(data);
    w.write_frame(data, 320 * 240);
    usleep(500 * 1000);
    
    color(data, 0xffff);
    w.write_frame(data, 320 * 240);
    usleep(500 * 1000);
    
    color(data, 0xf800);
    w.write_frame(data, 320 * 240);
    usleep(500 * 1000);  

    
    color(data, 0x07e0);
    w.write_frame(data, 320 * 240);
    usleep(500 * 1000);
    
    color(data, 0x001f);
    w.write_frame(data, 320 * 240);
    usleep(500 * 1000);
  }
}
