#include "screen.h"

#include <thread>
#include "st7789vi_frame_writer_smi.h"

namespace airball {

class ST7789VIScreen : public Screen {
public:
  ST7789VIScreen();

  ~ST7789VIScreen() override;

  virtual void flush() override {}

private:
  st7789vi_frame_writer_smi w_;
  unsigned char *data_;
  cairo_surface_t *cs_;
  cairo_t *cr_;
  std::thread paint_;
};

}  // namespace airball
