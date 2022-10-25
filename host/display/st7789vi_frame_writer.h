#ifndef ST7789VI_FRAME_WRITER_H
#define ST7789VI_FRAME_WRITER_H

namespace airball {

class st7789vi_frame_writer {
 public:
  st7789vi_frame_writer();
  virtual ~st7789vi_frame_writer();

  void initialize();
  
  void write(void* frame);
};

}  // namespace airball

#endif  // ST7789VI_FRAME_WRITER_H
