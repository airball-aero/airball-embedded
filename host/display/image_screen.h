#ifndef AIRBALL_IMAGE_SCREEN_H
#define AIRBALL_IMAGE_SCREEN_H

#include <string>
#include "screen.h"

namespace airball {

/**
 * Encapsulates a Screen which paints to an in-memory buffered image.
 */
class ImageScreen : public Screen {
public:

  /**
   * Creates a new ImageScreen.
   *
   * @param w the width of the Screen.
   * @param h the height of the Screen.
   */
  ImageScreen(int w, int h);

  virtual ~ImageScreen();

  virtual cairo_t* cr() const { return cr_; }

  virtual cairo_surface_t* cs() const { return cs_; }

  void write_to_png(std::string file_name) const;

private:

  cairo_t *cr_;
  cairo_surface_t *cs_;
};

}  // namespace airball

#endif //AIRBALL_IMAGE_SCREEN_H
