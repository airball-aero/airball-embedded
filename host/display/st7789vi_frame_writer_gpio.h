#ifndef ST7789VI_FRAME_WRITER_GPIO_H
#define ST7789VI_FRAME_WRITER_GPIO_H

#include "st7789vi_frame_writer.h"

namespace airball {

class st7789vi_frame_writer_gpio : public st7789vi_frame_writer {
 public:
  st7789vi_frame_writer_gpio() = default;
  virtual ~st7789vi_frame_writer_gpio() = default;

  void initialize() override;

protected:
  void write_data(uint16_t* data, int len) override;
};

}  // namespace airball

#endif  // ST7789VI_FRAME_WRITER_GPIO_H
