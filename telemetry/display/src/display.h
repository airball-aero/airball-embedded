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

#ifndef AIRBALL_DISPLAY_H
#define AIRBALL_DISPLAY_H

#include <cairo/cairo.h>

#include "airdata.h"
#include "screen.h"
#include "settings.h"

namespace airball {

class Display {
public:
  Display(Screen* screen, const Airdata* airdata, const Settings* settings)
      : screen_(screen), airdata_(airdata), settings_(settings) {}

  void paint();

private:
  void paintBackground();
  void paintAirball();
  void paintTotemPole();
  void paintTotemPoleLine();
  void paintTotemPoleAlphaX();
  void paintTotemPoleAlphaY();
  void paintCowCatcher();
  void paintAdjusting();

  double alpha_to_y(const double alpha);
  double beta_to_x(const double beta);

  double alpha_degrees_to_y(const double alpha_degrees);
  double beta_degrees_to_x(const double beta_degrees);

  double airspeed_to_radius(const double ias);

  Screen* screen_;
  const Airdata* airdata_;
  const Settings* settings_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_H
