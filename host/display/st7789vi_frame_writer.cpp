#include "st7789vi_frame_writer.h"

#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <pigpio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

constexpr unsigned char kPinDataCommand = 26; // raspi header pin 37
constexpr unsigned char kPinReset       = 27; // raspi header pin 13

namespace airball {

void st7789vi_frame_writer::initialize() {
  if (gpioInitialise() < 0) {
    exit(-1);
  }

  gpioSetMode(kPinDataCommand, PI_OUTPUT);
  gpioSetMode(kPinReset, PI_OUTPUT);

  delay(100);

  write_single_gpio(kPinReset, kPinStateLow);
  delay(250);
  write_single_gpio(kPinReset, kPinStateHigh);
  delay(250);

  command_out(0x28);   //display off
  command_out(0x11);  //exit sleep mode
  delay(100);

  command_out(0x36);  //madctl: memory data access control
  data_out(0x00);

  // 65k-colors in 16bit/pixel (5-6-5) format when using 16-bit interface
  command_out(0x3a);  //colmod: interface pixel format
  data_out(0x05);

  command_out(0xb2);  //porctrk: porch setting
  data_out(0x0c, 0x0c);
  data_out(0x00, 0x33);
  data_out(0x33);

  command_out(0xb7);  //gctrl: gate control
  data_out(0x35);

  command_out(0xbb);  //vcoms: vcom setting
  data_out(0x2b);

  command_out(0xc0);  //lcmctrl: lcm control
  data_out(0x2c);

  command_out(0xc2);  //vdvvrhen: vdv and vrh command enable
  data_out(0x01, 0xa1);

  command_out(0xe0);  //pvgamctrl: positive voltage gamma control
  data_out(0xd0, 0x00);
  data_out(0x05, 0x0e);
  data_out(0x15, 0x0d);
  data_out(0x37, 0x43);
  data_out(0x47, 0x09);
  data_out(0x15, 0x12);
  data_out(0x16, 0x19);

  command_out(0xe1);  //nvgamctrl: negative voltage gamma control
  data_out(0xd0, 0x00);
  data_out(0x05, 0x0d);
  data_out(0x0c, 0x06);
  data_out(0x2d, 0x44);
  data_out(0x40, 0x0e);
  data_out(0x1c, 0x18);
  data_out(0x16, 0x19);

  /*
  command_out(0x2a);  //x address set
  data_out(0x00, 0x00);
  data_out(0x00, 0xef);

  command_out(0x2b);  //y address set
  data_out(0x00, 0x00);
  data_out(0x01, 0x3f);
  delay(10);
  */
  
  command_out(0x29);  //display on
  delay(100);
}

void st7789vi_frame_writer::delay(uint16_t ms) {
  usleep(1000 * ms);
}

void st7789vi_frame_writer::write_single_gpio(unsigned char bit, unsigned char state) {
  if (bit > 31) {
    exit(-1);
  }
  const uint32_t s = 0x01 << bit;
  if (state) {
    gpioWrite_Bits_0_31_Set(s);
  } else {
    gpioWrite_Bits_0_31_Clear(s);
  }
  volatile uint32_t barrier = gpioRead_Bits_0_31();
}

void st7789vi_frame_writer::write_word(uint16_t b) {
  write_data(&b, 1);
}

void st7789vi_frame_writer::command_out(uint8_t c) {
  write_single_gpio(kPinDataCommand, kPinStateLow);
  write_word((uint16_t) c);
  for (int i = 0; i < 100; i++) {}
}

void st7789vi_frame_writer::data_out(uint16_t d) {
  write_single_gpio(kPinDataCommand, kPinStateHigh);
  write_word(d);
  for (int i = 0; i < 100; i++) {}
}

void st7789vi_frame_writer::data_out(uint8_t d0, uint8_t d1) {
  data_out((uint16_t) (d0 + (d1 << 8)));
}

void st7789vi_frame_writer::fail(const char *msg) {
  perror(msg);
  exit(1);
}

void st7789vi_frame_writer::write_frame(uint16_t* frame, int len) {
  command_out(0x2c);
  write_single_gpio(kPinDataCommand, kPinStateHigh);
  write_data(frame, len);
}
  
}  // namespace airball
