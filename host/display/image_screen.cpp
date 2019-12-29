#include "image_screen.h"

#include <cairo/cairo-xlib.h>

namespace airball {

ImageScreen::ImageScreen(int w, int h) {
  cs_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
  cr_ = cairo_create(cs_);
}

ImageScreen::~ImageScreen() {
  cairo_destroy(cr_);
  cairo_surface_destroy(cs_);
}

void ImageScreen::write_to_png(std::string file_name) const {
  cairo_surface_flush(cs_);
  cairo_surface_write_to_png(cs_, file_name.c_str());
}


}  // namespace airball