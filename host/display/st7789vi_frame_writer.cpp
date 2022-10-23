#include "st7789vi_frame_writer.h"

#include <pigpio.h>
#include <unistd.h>
#include <stdlib.h>

constexpr unsigned char kPinDataBit0    = 8;
constexpr unsigned char kPinDataCommand = 0;
constexpr unsigned char kPinWriteEnable = 1;
constexpr unsigned char kPinReadEnable  = 2;
constexpr unsigned char kPinReset       = 3;

constexpr unsigned char kPinStateLow  = 0;
constexpr unsigned char kPinStateHigh = 1;

constexpr int kDisplayWidth  = 240;
constexpr int kDisplayHeight = 320;
constexpr int kDisplayPixels = kDisplayWidth * kDisplayHeight;

static volatile uint32_t barrier;

void delay(uint16_t ms) {
  usleep(1000 * ms);
}

void setup_gpio_pins() {
  if (gpioInitialise() < 0) {
    exit(-1);
  }
  for (int i = 0; i < 16; i++) {
    gpioSetMode(i + kPinDataBit0, PI_OUTPUT);
  }
  gpioSetMode(kPinDataCommand, PI_OUTPUT);
  gpioSetMode(kPinWriteEnable, PI_OUTPUT);
  gpioSetMode(kPinReadEnable, PI_OUTPUT);
  gpioSetMode(kPinReset, PI_OUTPUT);
}

void write_single_gpio(unsigned char bit, unsigned char state) {
  if (bit > 31) {
    exit(-1);
  }
  const uint32_t s = 0x01 << bit;
  if (state) {
    gpioWrite_Bits_0_31_Set(s);
  } else {
    gpioWrite_Bits_0_31_Clear(s);
  }
  barrier = gpioRead_Bits_0_31();  
}

void write_word(uint16_t b) {
  uint16_t on = b;
  uint16_t off = ~b;
  write_single_gpio(kPinWriteEnable, kPinStateLow);
  gpioWrite_Bits_0_31_Clear(off << kPinDataBit0);
  gpioWrite_Bits_0_31_Set(on << kPinDataBit0);
  barrier = gpioRead_Bits_0_31();
  write_single_gpio(kPinWriteEnable, kPinStateHigh);
  barrier = gpioRead_Bits_0_31();  
}

void comm_out(uint8_t c)
{
  write_single_gpio(kPinDataCommand, kPinStateLow);
  write_word((uint16_t) c);
}

void data_out(uint16_t d)
{
  write_single_gpio(kPinDataCommand, kPinStateHigh);
  write_word(d);
}

void data_out(uint8_t d0, uint8_t d1) {
  data_out(d0 + d1 << 8);
}

namespace airball {

st7789vi_frame_writer::st7789vi_frame_writer() {}

st7789vi_frame_writer::~st7789vi_frame_writer() {}

void st7789vi_frame_writer::initialize() {
  setup_gpio_pins();
  
  write_single_gpio(kPinReadEnable, kPinStateHigh);
  write_single_gpio(kPinWriteEnable, kPinStateLow);
  write_single_gpio(kPinReset, kPinStateLow);
  delay(250);
  write_single_gpio(kPinReset, kPinStateHigh);
  delay(250);
  
  comm_out(0x28);   //display off
  comm_out(0x11);  //exit SLEEP mode
  delay(100);
  
  comm_out(0x36);  //MADCTL: memory data access control
  data_out(0x00);

  // 65K-colors in 16bit/pixel (5-6-5) format when using 16-bit interface
  comm_out(0x3A);  //COLMOD: Interface Pixel format
  data_out(0x05);

  comm_out(0xB2);  //PORCTRK: Porch setting
  data_out(0x0C, 0x0C);
  data_out(0x00, 0x33);
  data_out(0x33);

  comm_out(0xB7);  //GCTRL: Gate Control
  data_out(0x35);

  comm_out(0xBB);  //VCOMS: VCOM setting
  data_out(0x2B);

  comm_out(0xC0);  //LCMCTRL: LCM Control
  data_out(0x2C);

  comm_out(0xC2);  //VDVVRHEN: VDV and VRH Command Enable
  data_out(0x01, 0xA1);

  comm_out(0xE0);  //PVGAMCTRL: Positive Voltage Gamma control  
  data_out(0xD0, 0x00);
  data_out(0x05, 0x0E);
  data_out(0x15, 0x0D);
  data_out(0x37, 0x43);
  data_out(0x47, 0x09);
  data_out(0x15, 0x12);
  data_out(0x16, 0x19);
  
  comm_out(0xE1);  //NVGAMCTRL: Negative Voltage Gamma control  
  data_out(0xD0, 0x00);
  data_out(0x05, 0x0D);
  data_out(0x0C, 0x06);
  data_out(0x2D, 0x44);
  data_out(0x40, 0x0E);
  data_out(0x1C, 0x18);
  data_out(0x16, 0x19);
    
  comm_out(0x2A);  //X address set
  data_out(0x00, 0x00);
  data_out(0x00, 0xEF);

  comm_out(0x2B);  //Y address set
  data_out(0x00, 0x00);
  data_out(0x01, 0x3F);
  delay(10);
  
  comm_out(0x29);  //display ON
  delay(10);
}    
  
void st7789vi_frame_writer::write(uint16_t* frame) {
  comm_out(0x2C);
  write_single_gpio(kPinDataCommand, kPinStateHigh);
  for (int i = 0; i < kDisplayPixels; i++) {
    write_word(frame[i]);
  }
}    
  
}  // namespace airball
