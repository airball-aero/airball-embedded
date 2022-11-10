#include "st7789vi_screen.h"

#include <iostream>
#include <stdint.h>
#include <unistd.h>

namespace airball {

ST7789VIScreen::ST7789VIScreen() {
  w_.initialize();
  data_ = (unsigned char *) malloc(sizeof(uint16_t) * 320 * 240);
  set_cs(cairo_image_surface_create_for_data(
      data_,
      CAIRO_FORMAT_RGB16_565,
      240,
      320,
      cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, 240)));
  set_cr(cairo_create(cs()));
  paint_ = std::thread([&]() {
    while (true) {
      w_.write_frame((uint16_t *) data_, 320 * 240);
      usleep(1000);
    }
  });
}

ST7789VIScreen::~ST7789VIScreen() {
  cairo_destroy(cr());
  cairo_surface_destroy(cs());
  free(data_);
  // TODO: Join paint_ thread, etc.
}

}  // namespace airball
