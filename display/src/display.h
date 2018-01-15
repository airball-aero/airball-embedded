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
  double airspeed_to_radius(const double ias);

  Screen* screen_;
  const Airdata* airdata_;
  const Settings* settings_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_H
