#ifndef AIRBALL_SCREEN_H
#define AIRBALL_SCREEN_H

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
  virtual cairo_t* cr() const = 0;

  /**
   * @return a Cairo surface corresponding to the context cr().
   */
  virtual cairo_surface_t* cs() const = 0;

  /**
   * @deprecated this is X11 specific; use UserInputSource instead!!
   * @return the next event.
   */
  virtual int next_event() const = 0;

  /**
   * Creates a Screen that draws to an X11 window.
   *
   * @param x the X size of the window.
   * @param y the Y size of the window.
   *
   * @return a Screen.
   */
  static Screen* NewX11Screen(const int x, const int y);

  /**
   * Creates a Screen that draws to the current Linux frame buffer.
   *
   * @return a Screen.
   */
  static Screen* NewFramebufferScreen();
};

}  // namespace airball

#endif //AIRBALL_SCREEN_H
