#include "st7789vi_frame_writer_gpio.h"

#include <pigpio.h>

constexpr static unsigned char kPinFirstDataGpio = 8;
constexpr static unsigned char kPinWrite         = 7;

namespace airball {

void st7789vi_frame_writer_gpio::initialize() {
  st7789vi_frame_writer::initialize();
  for (int i = 0; i < 16; i++) {
    gpioSetMode(i + kPinFirstDataGpio, PI_OUTPUT);
  }
  gpioSetMode(kPinWrite, PI_OUTPUT);
}

void st7789vi_frame_writer_gpio::write_data(uint16_t* buf, int len) {
  for (int i = 0; i < len; i++) {
    uint16_t on = buf[i];
    uint16_t off = ~on;
    write_single_gpio(kPinWrite, kPinStateLow);
    volatile uint32_t barrier = gpioRead_Bits_0_31();    
    gpioWrite_Bits_0_31_Clear(off << kPinFirstDataGpio);
    gpioWrite_Bits_0_31_Set(on << kPinFirstDataGpio);
    barrier = gpioRead_Bits_0_31();
    write_single_gpio(kPinWrite, kPinStateHigh);
    barrier = gpioRead_Bits_0_31();
  }
}

}  // namespace airball
