#ifndef AIRBALL_DISPLAY_H
#define AIRBALL_DISPLAY_H

#include <cairo/cairo.h>

#include "airdata.h"
#include "screen.h"
#include "settings.h"
#include "system_status.h"
#include "widgets.h"

namespace airball {

class Display {
public:
  Display(Screen* screen,
          const Airdata* airdata,
          const Settings* settings,
          const SystemStatus* status)
      : screen_(screen), airdata_(airdata), settings_(settings), status_(status)
      {}

  void paint();

private:
  void paintBackground();
  void paintRawAirballs();
  void paintRawAirball(
      const Point& center,
      const double radius,
      const double bright);
  void paintSmoothAirball();
  void paintAirballLowAirspeed(const Point& center);
  void paintAirballAirspeed(const Point& center, const double radius);
  void paintAirballAirspeedText(const Point& center, const double ias);
  void paintAirballAirspeedLimits(const Point& center);
  void paintAirballAirspeedLimitsNormal(const Point& center);
  void paintAirballAirspeedLimitsRotate(const Point& center);
  void paintAirballTrueAirspeed(const Point& center);
  void paintTotemPole();
  void paintTotemPoleLine();
  void paintTotemPoleAlphaX();
  void paintTotemPoleAlphaY();
  void paintCowCatcher();
  void paintVsi();
  void paintVsiTicMarks(
      Point top_left,
      Point top_right,
      Point center_left,
      Point center_right,
      Point bottom_left,
      Point bottom_right,
      double radians_per_fpm);
  void paintVsiPointer(
      Point top_left,
      Point top_right,
      Point center_left,
      Point center_right,
      Point bottom_left,
      Point bottom_right,
      double radians_per_fpm);
  void paintAltitude(
      Point top_left,
      Point top_right,
      Point center_left,
      Point center_right,
      Point bottom_left,
      Point bottom_right);
  void paintBaroSetting(
      Point top_left,
      Point top_right,
      Point center_left,
      Point center_right,
      Point bottom_left,
      Point bottom_right);
  void paintNoFlightData();
  void paintBatteryStatus();
  void paintLinkStatus();

  double alpha_to_y(const double alpha);
  double beta_to_x(const double beta);

  double alpha_degrees_to_y(const double alpha_degrees);
  double beta_degrees_to_x(const double beta_degrees);

  double airspeed_to_radius(const double ias);
  double airspeed_knots_to_radius(const double airspeed_knots);

  Screen* screen_;
  const Airdata* airdata_;
  const Settings* settings_;
  const SystemStatus* status_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_H
