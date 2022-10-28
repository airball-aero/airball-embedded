#ifndef AIRBALL_ISCREEN_H
#define AIRBALL_ISCREEN_H

#include <cairo/cairo.h>

namespace airball {

/**
 * Encapsulates a physical screen to which an Airball Display can draw its
 * user interface.
 */
class Screen {
public:

  virtual ~Screen() {};

  /**
   * @return a Cairo context allowing an application to draw to the Screen.
   */
  cairo_t *cr() const { return cr_; }

  /**
   * @return a Cairo surface corresponding to the context cr().
   */
  cairo_surface_t *cs() const { return cs_; }

  /**
   * Indicate to this Screen that the current image is to be flushed.
   */
  virtual void flush() = 0;

protected:
  void set_cr(cairo_t* cr) { cr_ = cr; }

  void set_cs(cairo_surface_t* cs) { cs_ = cs; }

private:
  cairo_t *cr_;
  cairo_surface_t *cs_;
};

}  // namespace airball

#endif //AIRBALL_ISCREEN_H
