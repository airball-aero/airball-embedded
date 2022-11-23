#include "st7789vi_screen.h"

#include <iostream>
#include <stdint.h>
#include <unistd.h>

namespace airball {

constexpr uint32_t kWidth = 240;
constexpr uint32_t kHeight = 320;
  
ST7789VIScreen::ST7789VIScreen() {
  w_.initialize();
  data_ = (unsigned char *) malloc(sizeof(uint16_t) * kWidth * kHeight);
  set_cs(cairo_image_surface_create_for_data(
      data_,
      CAIRO_FORMAT_RGB16_565,
      kWidth,
      kHeight,
      cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, kWidth)));
  set_cr(cairo_create(cs()));
  paint_ = std::thread([&]() {
    while (true) {
      std::unique_lock<decltype(paint_mu_)> lock(paint_mu_);
      paint_cond_.wait(lock);
      w_.write_frame((uint16_t *) data_, kWidth * kHeight);
    }
  });
}

ST7789VIScreen::~ST7789VIScreen() {
  flush();
  paint_.join();
  cairo_destroy(cr());
  cairo_surface_destroy(cs());
  free(data_);
}

void ST7789VIScreen::flush() {
  std::lock_guard<decltype(paint_mu_)> lock(paint_mu_);
  paint_cond_.notify_one();
}
  
}  // namespace airball
