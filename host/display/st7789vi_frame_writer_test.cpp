#include "st7789vi_frame_writer.h"

void color(uint16_t* data, uint16_t color) {
  for(int i = 0; i < 320 * 240; i++) {
    data[i] = color;
  }
}

void loop() {
  airball::st7789vi_frame_writer w;
  w.initialize();

  uint16_t data[320 * 240];

  color(data, 0xffff);
  w.write(data);
  color(data, 0xf800);
  w.write(data);
  color(data, 0x07e0);
  w.write(data);
  color(data, 0x001f);
  w.write(data);
}

int main(int argc, char**argv) {
  while (true) {
    loop();
  }
}
