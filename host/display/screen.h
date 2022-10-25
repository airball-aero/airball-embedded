/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018, Ihab A.B. Awad
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
   * Indicate to this Screen that the current image is to be flushed.
   */
  virtual void flush() = 0;

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

  /**
   * Creates a Screen that draws to an ST7789VI connected display.
   *
   * @return a Screen.
   */
  static Screen* NewST7789VIScreen();
};

}  // namespace airball

#endif //AIRBALL_SCREEN_H
