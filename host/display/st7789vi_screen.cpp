#include "st7789vi_screen.h"

#include <iostream>
#include <stdint.h>
#include <unistd.h>

namespace airball {

ST7789VIScreen::ST7789VIScreen() {
  w_.initialize();
  data_ = (unsigned char *) malloc(sizeof(uint16_t) * 320 * 240);
  std::cout << "data_ = " << data_ << std::endl;
  cs_ = cairo_image_surface_create_for_data(
      data_,
      CAIRO_FORMAT_RGB16_565,
      240,
      320,
      cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, 240));
  cr_ = cairo_create(cs_);
  std::cout << "cr_ = " << cr_ << std::endl;
  paint_ = std::thread([&]() {
    while (true) {
      w_.write_frame((uint16_t *) data_, 320 * 240);
      usleep(100 * 1000);
    }
  });
}

ST7789VIScreen::~ST7789VIScreen() {
  cairo_destroy(cr_);
  cairo_surface_destroy(cs_);
  free(data_);
  // TODO: Join paint_ thread, etc.
}

}  // namespace airball
