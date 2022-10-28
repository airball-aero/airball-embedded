#ifndef ST7789VI_FRAME_WRITER_H
#define ST7789VI_FRAME_WRITER_H

#include <stdint.h>
#include <cstddef>

namespace airball {

class st7789vi_frame_writer {
 public:
  st7789vi_frame_writer() = default;
  virtual ~st7789vi_frame_writer() = default;

  virtual void initialize();

  void write_frame(uint16_t* frame, int len);

protected:
  constexpr static unsigned char kPinStateLow  = 0;
  constexpr static unsigned char kPinStateHigh = 1;

  void write_single_gpio(unsigned char bit, unsigned char state);
  void fail(const char *msg);

  virtual void write_data(uint16_t* buf, int len) = 0;

private:
  void command_out(uint8_t c);
  void delay(uint16_t ms);
  void write_word(uint16_t b);
  void data_out(uint16_t d);
  void data_out(uint8_t d0, uint8_t d1);
};

}  // namespace airball

#endif  // ST7789VI_FRAME_WRITER_H
