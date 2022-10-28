#ifndef ST7789VI_FRAME_WRITER_SMI_H
#define ST7789VI_FRAME_WRITER_SMI_H

#include "st7789vi_frame_writer.h"

namespace airball {

class st7789vi_frame_writer_smi : public st7789vi_frame_writer {
 public:
  st7789vi_frame_writer_smi() = default;
  virtual ~st7789vi_frame_writer_smi();

  void initialize() override;

protected:
  void write_data(uint16_t* data, int len) override;

private:
  int fd_;
};

}  // namespace airball

#endif  // ST7789VI_FRAME_WRITER_SMI_H
