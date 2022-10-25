#include "st7789vi_frame_writer.h"

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
  airball::st7789vi_frame_writer w;
  w.initialize();

  uint16_t data[320 * 240];
  
  while (true) {
    alternating_bits(data);    
    w.write(data);
    usleep(500 * 1000);
    
    color(data, 0xffff);
    w.write(data);
    usleep(500 * 1000);
    
    color(data, 0xf800);
    w.write(data);
    usleep(500 * 1000);  
    
    color(data, 0x07e0);
    w.write(data);
    usleep(500 * 1000);
    
    color(data, 0x001f);
    w.write(data);
    usleep(500 * 1000);
  }
}
