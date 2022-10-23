#ifndef ST7789VI_FRAME_WRITER_H
#define ST7789VI_FRAME_WRITER_H

#include <stdint.h>

namespace airball {

class st7789vi_frame_writer {
 public:
  st7789vi_frame_writer();
  virtual ~st7789vi_frame_writer();

  void initialize();
  
  void write(uint16_t* frame);
};

}  // namespace airball

#endif  // ST7789VI_FRAME_WRITER_H
