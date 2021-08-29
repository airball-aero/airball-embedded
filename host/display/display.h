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
          const SystemStatus* status);

  void paint();

private:

  struct VsiStep {
    double fpm;
    double thick;
  };

  std::string fontName_;
  double width_ = 272;
  double height_ = 480;
  double altimeterHeight_;
  double airballHeight_;
  double displayMargin_;
  double topBottomRegionRatio_;
  double displayXMid_;
  double displayRegionYMin_;
  double displayRegionYMax_;
  double displayRegionHeight_;
  double displayRegionWidth_;
  double displayRegionHalfWidth_;
  double speedLimitsRosetteHalfAngle_;
  double trueAirspeedRosetteHalfAngle_;
  double alphaRefRadius_;
  double alphaRefGapDegrees_;
  double lowSpeedThresholdAirballRadius_;
  double alphaRefTopAngle0_;
  double alphaRefTopAngle1_;
  double alphaRefBotAngle0_;
  double alphaRefBotAngle1_;
  double totemPoleAlphaUnit_;
  int numCowCatcherLines_;
  Color background_;
  Color airballFill_;
  double rawAirballsMaxBrightness_;
  Stroke airballCrosshairsStroke_;
  double lowSpeedAirballStrokeWidth_;
  Color tasRingColor_;
  double tasRingStrokeWidth_;
  double tasThresholdRatio_;
  Stroke lowSpeedAirballStroke_;
  double lowSpeedAirballArcRadius_;
  Stroke totemPoleStroke_;
  Stroke cowCatcherStroke_;
  Stroke vfeStroke_;
  Stroke vnoStroke_;
  Stroke vneStroke_;
  Stroke vBackgroundStroke_;
  double iASTextFontSize_;
  Font iASTextFont_;
  double iASTextMargin_;
  Color iASTextColor_;
  int printBufSize_;
  Stroke noFlightDataStroke_;
  double statusRegionMargin_;
  double statusDisplayUnit_;
  double statusDisplayStrokeWidth_;
  Stroke statusDisplayStroke_;
  double statusTextFontSize_;
  Font statusTextFont_;
  Color statusTextColor_;
  Color batteryColorGood_;
  Color batteryColorWarning_;
  Color batteryColorBad_;
  bool statusDisplayNumericalData_;
  Color linkColor_;
  double vsiHeight_;
  double vsiPrecisionFpm_;
  double vsiMaxFpm_;
  std::vector<VsiStep> vsiStepsFpm_;
  double vsiTickLength_;
  double vsiKneeOffset_;
  Stroke vsiTickStrokeThin_;
  Stroke vsiPointerStroke_;
  Font altimeterFontLarge_;
  Font altimeterFontSmall_;
  Color altimeterTextColor_;
  Color altimeterBackgroundColor_;
  double altimeterBaselineRatio_;
  double altimeterNumberGap_;
  double baroLeftOffset_;
  Font baroFontSmall_;
  Color baroTextColor_;

  void layout();
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
