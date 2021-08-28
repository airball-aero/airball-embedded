#include "display.h"

#include <math.h>
#include <iostream>

#include "units.h"
#include "widgets.h"

namespace airball {

///////////////////////////////////////////////////////////////////////

constexpr double ce_floor(double x) {
  return static_cast<double>(static_cast<int64_t>(x));
}

constexpr char kFontName[] =
    "Noto Sans";

constexpr double kWidth = 272;
constexpr double kHeight = 480;

constexpr double kAltimeterHeight = 48;
constexpr double kAirballHeight = kHeight - kAltimeterHeight;

constexpr double kDisplayMargin = 3;

constexpr double kTopBottomRegionRatio = 0.075;

constexpr double kDisplayXMid = ce_floor(kWidth / 2.0);

constexpr double kDisplayRegionYMin = ce_floor(kAirballHeight * kTopBottomRegionRatio);
constexpr double kDisplayRegionYMax = ce_floor(kAirballHeight * (1.0 - kTopBottomRegionRatio));
constexpr double kDisplayRegionHeight = kDisplayRegionYMax - kDisplayRegionYMin;

constexpr double kDisplayRegionWidth = kWidth;
constexpr double kDisplayRegionHalfWidth = kDisplayRegionWidth / 2;

constexpr double kSpeedLimitsRosetteHalfAngle  = (15.0 / 2 / 180 * M_PI);
constexpr double kTrueAirspeedRosetteHalfAngle = (70.0 / 2 / 180 * M_PI);

constexpr double kAlphaRefRadius = 20;

constexpr double kAlphaRefGapDegrees = 40;

constexpr double kLowSpeedThresholdAirballRadius = 0.1 * (kWidth / 2.0);

constexpr double kAlphaRefTopAngle0 = M_PI + (kAlphaRefGapDegrees / 2 / 180 * M_PI);
constexpr double kAlphaRefTopAngle1 =      - (kAlphaRefGapDegrees / 2 / 180 * M_PI);

constexpr double kAlphaRefBotAngle0 =        (kAlphaRefGapDegrees / 2 / 180 * M_PI);
constexpr double kAlphaRefBotAngle1 = M_PI - (kAlphaRefGapDegrees / 2 / 180 * M_PI);

constexpr double kTotemPoleAlphaUnit = 20;

constexpr int kNumCowCatcherLines = 3;

constexpr Color kBackground(0, 0, 0);

constexpr Color kAirballFill(255, 255, 255);

constexpr double kRawAirballsMaxBrightness = 0.375;

constexpr Stroke kAirballCrosshairsStroke(
    Color(128, 128, 128),
    2);

constexpr double kLowSpeedAirballStrokeWidth = 4.0;

constexpr Color kTasRingColor(255, 0, 255);

constexpr double kTasRingStrokeWidth = 3.0;

constexpr double kTasThresholdRatio = 0.25;

constexpr Stroke kLowSpeedAirballStroke(
    Color(255, 255, 255),
    kLowSpeedAirballStrokeWidth);

constexpr double kLowSpeedAirballArcRadius =
    kLowSpeedThresholdAirballRadius - kLowSpeedAirballStrokeWidth / 2.0;

constexpr Stroke kTotemPoleStroke(
    Color(255, 255, 0),
    3);

constexpr Stroke kCowCatcherStroke(
    Color(255, 0, 0),
    3);

constexpr Stroke kVfeStroke(
    Color(255, 255, 255),
    3);

constexpr Stroke kVnoStroke(
    Color(255, 255, 0),
    3);

constexpr Stroke kVneStroke(
    Color(255, 0, 0),
    3);

constexpr Stroke kVBackgroundStroke(
    Color(0, 0, 0),
    6);

constexpr double kIASTextFontSize =
    kWidth / 5.0;

constexpr Font kIASTextFont(
    kFontName,
    kIASTextFontSize);

constexpr double kIASTextMargin =
    2;

constexpr Color kIASTextColor(0, 0, 0);

constexpr double kAdjustingTextFontSize = kWidth / 14;
constexpr double kAdjustingRegionWidth = kAdjustingTextFontSize * 10;
constexpr double kAdjustingRegionHeight = kAdjustingTextFontSize + 8;

constexpr int kPrintBufSize = 128;

constexpr Stroke kNoFlightDataStroke(
    Color(255, 0, 0),
    3);

constexpr double kStatusRegionMargin = 5;

constexpr double kStatusDisplayUnit = 20;

constexpr double kStatusDisplayStrokeWidth = 2;

constexpr Stroke kStatusDisplayStroke(
    Color(128, 128, 128),
    kStatusDisplayStrokeWidth);

constexpr double kStatusTextFontSize = 12;

constexpr Font kStatusTextFont(
    kFontName,
    kStatusTextFontSize);

constexpr Color kStatusTextColor(200, 200, 200);

constexpr Color kBatteryColorGood(0, 180, 0);

constexpr Color kBatteryColorWarning(255, 255, 0);

constexpr Color kBatteryColorBad(255, 0, 0);

const bool kStatusDisplayNumericalData = false;

constexpr Color kLinkColor(0, 180, 180);

constexpr double kVsiHeight = kAltimeterHeight - 2 * kDisplayMargin;
constexpr double kVsiPrecisionFpm = 100;
constexpr double kVsiMaxFpm = 2000;
constexpr struct {
  double fpm;
  double thick;
} kVsiStepsFpm[]{
    {
        .fpm = 200,
        .thick = 1,
    },
    {
        .fpm = 300,
        .thick = 1,
    },
    {
      .fpm = 400,
      .thick = 1,
    },
    {
      .fpm = 500,
      .thick = 1.5,
    },
    {
        .fpm = 1000,
        .thick = 2,
    },
    {
      .fpm = 1500,
      .thick = 1.5,
    },
};
constexpr double kVsiTickLength = 7;
constexpr double kVsiKneeOffset = 3;
constexpr Stroke kVsiTickStrokeThin(
    Color(255, 255, 255),
    2);
constexpr Stroke kVsiPointerStroke(
    Color(255, 0, 255),
    4);
constexpr Font kAltimeterFontLarge(
    kFontName,
    kWidth / 8);
constexpr Font kAltimeterFontSmall(
    kFontName,
    kWidth / 12);
constexpr Color kAltimeterTextColor(255, 255, 255);
constexpr Color kAltimeterBackgroundColor(64, 64, 64);
constexpr double kAltimeterBaselineRatio = 0.75;
constexpr double kAltimeterNumberGap = 7;

constexpr double kBaroLeftOffset =
    kWidth / 12;
constexpr Font kBaroFontSmall(
    kFontName,
    kWidth / 20);
constexpr Color kBaroTextColor(255, 255, 255);

///////////////////////////////////////////////////////////////////////

double Display::alpha_to_y(const double alpha) {
  return alpha_degrees_to_y(radians_to_degrees(alpha));
}

double Display::alpha_degrees_to_y(const double alpha_degrees) {
  double ratio = (alpha_degrees - settings_->alpha_min())
                 / (settings_->alpha_stall() - settings_->alpha_min());
  return kDisplayRegionYMin + ratio * kDisplayRegionHeight;
}

double Display::beta_to_x(const double beta) {
  return beta_degrees_to_x(radians_to_degrees(beta));
}

double Display::beta_degrees_to_x(const double beta_degrees) {
  double ratio = (beta_degrees + settings_->beta_bias()) / settings_->beta_full_scale();
  return kDisplayRegionHalfWidth * (1.0 + ratio);
}

double Display::airspeed_to_radius(const double airspeed) {
  return airspeed_knots_to_radius(meters_per_second_to_knots(airspeed));
}

double Display::airspeed_knots_to_radius(const double airspeed_knots) {
  double ratio = airspeed_knots / settings_->v_full_scale();
  return ratio * kWidth / 2;
}

void Display::paint() {
  cairo_push_group(screen_->cr());
  cairo_translate(screen_->cr(), 0, kWidth);
  cairo_rotate(screen_->cr(), -M_PI / 2);

  paintBackground();

  cairo_save(screen_->cr());
  cairo_rectangle(screen_->cr(), 0, 0, kWidth, kAirballHeight);
  cairo_clip(screen_->cr());
  if (status_->flight_data_up()) {
    paintRawAirballs();
    paintSmoothAirball();
  } else {
    paintNoFlightData();
  }
  paintTotemPole();
  paintCowCatcher();
  paintBatteryStatus();
  paintLinkStatus();
  cairo_restore(screen_->cr());

  paintVsi();

  cairo_pop_group_to_source(screen_->cr());
  cairo_paint(screen_->cr());
  cairo_surface_flush(screen_->cs());
}

void Display::paintBackground() {
  rectangle(
      screen_->cr(),
      Point(0, 0),
      Size(kWidth, kHeight),
      kBackground);
}

void Display::paintRawAirballs() {
  for (uint i = airdata_->raw_balls().size(); i-- > 0; ) {
    uint bright_index = airdata_->raw_balls().size() - i;
    double bright =
        ((double) bright_index) / ((double) airdata_->raw_balls().size()) *
        kRawAirballsMaxBrightness;
    Point center(
        beta_to_x(airdata_->raw_balls()[i].beta()),
        alpha_to_y((airdata_->raw_balls()[i].alpha())));
    double radius = airspeed_to_radius(airdata_->raw_balls()[i].ias());
    paintRawAirball(center, radius, bright);
  }
}

void Display::paintRawAirball(
    const Point& center,
    const double radius,
    const double bright) {
  disc(
      screen_->cr(),
      center,
      radius,
      kAirballFill.with_brightness(bright));
}

void Display::paintSmoothAirball() {
  Point center(beta_to_x(airdata_->smooth_ball().beta()), alpha_to_y((airdata_->smooth_ball().alpha())));
  double radius = airspeed_to_radius(airdata_->smooth_ball().ias());
  if (radius < kLowSpeedThresholdAirballRadius) {
    paintAirballLowAirspeed(center);
  } else {
    paintAirballAirspeed(center, radius);
    if ((radius * 2) > (kIASTextFontSize * 1.5)) {
      paintAirballAirspeedText(center, airdata_->smooth_ball().ias());
    }
    paintAirballTrueAirspeed(center);
    paintAirballAirspeedLimits(center);
  }
}

void Display::paintAirballLowAirspeed(const Point& center) {
  arc(
      screen_->cr(),
      center,
      kLowSpeedAirballArcRadius,
      0,
      2.0 * M_PI,
      kLowSpeedAirballStroke);
}

void Display::paintAirballAirspeed(const Point& center, const double radius) {
  disc(
      screen_->cr(),
      center,
      radius,
      kAirballFill);
  line(
      screen_->cr(),
      Point(center.x(), center.y() - radius),
      Point(center.x(), center.y() + radius),
      kAirballCrosshairsStroke);
  line(
      screen_->cr(),
      Point(center.x() - radius, center.y()),
      Point(center.x() + radius, center.y()),
      kAirballCrosshairsStroke);
}

void Display::paintAirballAirspeedText(const Point& center, const double ias) {
  char buf[kPrintBufSize];
  double ias_knots = meters_per_second_to_knots(ias);
  snprintf(
      buf,
      kPrintBufSize,
      "%.0f",
      ias_knots);
  Size sz = text_size(screen_->cr(), buf, kIASTextFont);
  rectangle(
      screen_->cr(),
      Point(
          center.x() - sz.w() / 2 - kIASTextMargin,
          center.y() - sz.h() / 2 - kIASTextMargin),
      Size(
          sz.w() + 2 * kIASTextMargin,
          sz.h() + 2 * kIASTextMargin),
      kAirballFill);
  text(
      screen_->cr(),
      buf,
      center,
      TextReferencePoint::CENTER_MID_UPPERCASE,
      kIASTextFont,
      kIASTextColor);
}

void Display::paintAirballAirspeedLimits(const Point& center) {
  if (meters_per_second_to_knots(airdata_->ias()) < settings_->v_r()) {
    paintAirballAirspeedLimitsRotate(center);
  } else {
    paintAirballAirspeedLimitsNormal(center);
  }
}

void Display::paintAirballAirspeedLimitsRotate(const Point& center) {
  double r = airspeed_knots_to_radius(settings_->v_r());
  line(
      screen_->cr(),
      Point(
          center.x() - r,
          center.y()),
      Point(
          center.x() - r - kTotemPoleAlphaUnit,
          center.y() + kTotemPoleAlphaUnit),
      kAirballCrosshairsStroke);
  line(
      screen_->cr(),
      Point(
          center.x() - r,
          center.y()),
      Point(
          center.x() - r - kTotemPoleAlphaUnit,
          center.y() - kTotemPoleAlphaUnit),
      kAirballCrosshairsStroke);
  line(
      screen_->cr(),
      Point(
          center.x() + r,
          center.y()),
      Point(
          center.x() + r + kTotemPoleAlphaUnit,
          center.y() + kTotemPoleAlphaUnit),
      kAirballCrosshairsStroke);
  line(
      screen_->cr(),
      Point(
          center.x() + r,
          center.y()),
      Point(
          center.x() + r + kTotemPoleAlphaUnit,
          center.y() - kTotemPoleAlphaUnit),
      kAirballCrosshairsStroke);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() + r),
      Point(
          center.x() + kTotemPoleAlphaUnit,
          center.y() + r + kTotemPoleAlphaUnit),
      kAirballCrosshairsStroke);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() + r),
      Point(
          center.x() - kTotemPoleAlphaUnit,
          center.y() + r + kTotemPoleAlphaUnit),
      kAirballCrosshairsStroke);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() - r),
      Point(
          center.x() + kTotemPoleAlphaUnit,
          center.y() - r - kTotemPoleAlphaUnit),
      kAirballCrosshairsStroke);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() - r),
      Point(
          center.x() - kTotemPoleAlphaUnit,
          center.y() - r - kTotemPoleAlphaUnit),
      kAirballCrosshairsStroke);
}

void Display::paintAirballAirspeedLimitsNormal(const Point& center) {
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_fe()),
      4,
      kSpeedLimitsRosetteHalfAngle,
      M_PI_4,
      kVBackgroundStroke);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_fe()),
      4,
      kSpeedLimitsRosetteHalfAngle,
      M_PI_4,
      kVfeStroke);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_no()),
      4,
      kSpeedLimitsRosetteHalfAngle,
      M_PI_4,
      kVBackgroundStroke);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_no()),
      4,
      kSpeedLimitsRosetteHalfAngle,
      M_PI_4,
      kVnoStroke);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_ne()),
      4,
      kSpeedLimitsRosetteHalfAngle,
      M_PI_4,
      kVBackgroundStroke);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_ne()),
      4,
      kSpeedLimitsRosetteHalfAngle,
      M_PI_4,
      kVneStroke);
}

void Display::paintAirballTrueAirspeed(const Point& center) {
  double tas_stroke_alpha = 0;
  if (airdata_->smooth_ball().tas() <
      airdata_->smooth_ball().ias() || airdata_->smooth_ball().ias() == 0) {
    tas_stroke_alpha = 0;
  } else {
    double ias_squared =
        airdata_->smooth_ball().ias() * airdata_->smooth_ball().ias();
    double tas_squared =
        airdata_->smooth_ball().tas() * airdata_->smooth_ball().tas();
    double ratio = (tas_squared - ias_squared) / ias_squared;
    tas_stroke_alpha = (ratio > kTasThresholdRatio)
                       ? 1.0 : (ratio / kTasThresholdRatio);
  }
  rosette(
      screen_->cr(),
      center,
      airspeed_to_radius(airdata_->smooth_ball().tas()),
      4,
      kTrueAirspeedRosetteHalfAngle,
      0,
      Stroke(
          kTasRingColor.with_alpha(tas_stroke_alpha),
          kTasRingStrokeWidth));
}

void Display::paintTotemPole() {
  paintTotemPoleLine();
  paintTotemPoleAlphaX();
  paintTotemPoleAlphaY();
}

void Display::paintTotemPoleLine() {
  line(
      screen_->cr(),
      Point(kDisplayXMid, 0),
      Point(kDisplayXMid, alpha_degrees_to_y(settings_->alpha_ref()) - kAlphaRefRadius),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(kDisplayXMid, alpha_degrees_to_y(settings_->alpha_ref()) + kAlphaRefRadius),
      Point(kDisplayXMid, kDisplayRegionYMax),
      kTotemPoleStroke);
  arc(
      screen_->cr(),
      Point(kDisplayXMid, alpha_degrees_to_y(settings_->alpha_ref())),
      kAlphaRefRadius,
      kAlphaRefTopAngle0,
      kAlphaRefTopAngle1,
      kTotemPoleStroke);
  arc(
      screen_->cr(),
      Point(kDisplayXMid, alpha_degrees_to_y(settings_->alpha_ref())),
      kAlphaRefRadius,
      kAlphaRefBotAngle0,
      kAlphaRefBotAngle1,
      kTotemPoleStroke);
}

void Display::paintTotemPoleAlphaX() {
  line(
      screen_->cr(),
      Point(
          kDisplayXMid - 3 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_x())),
      Point(
          kDisplayXMid - 2 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_x())),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid - 2 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_x())),
      Point(
          kDisplayXMid - 3 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_x()) - kTotemPoleAlphaUnit) ,
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid + 3 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_x())),
      Point(
          kDisplayXMid + 2 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_x())),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid + 2 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_x())),
      Point(
          kDisplayXMid + 3 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_x()) - kTotemPoleAlphaUnit) ,
      kTotemPoleStroke);
}

void Display::paintTotemPoleAlphaY() {
  line(
      screen_->cr(),
      Point(
          kDisplayXMid - 4 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_y())),
      Point(
          kDisplayXMid - 5 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_y())),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid - 5 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_y())),
      Point(
          kDisplayXMid - 6 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_y()) - kTotemPoleAlphaUnit),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid + 4 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_y())),
      Point(
          kDisplayXMid + 5 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_y())),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid + 5 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_y())),
      Point(
          kDisplayXMid + 6 * kTotemPoleAlphaUnit,
          alpha_degrees_to_y(settings_->alpha_y()) - kTotemPoleAlphaUnit),
      kTotemPoleStroke);
}

void Display::paintCowCatcher() {
  double xStep =
      (kDisplayRegionWidth - (2 * kDisplayMargin)) /
      (2 * kNumCowCatcherLines);
  for (int i = 0; i < kNumCowCatcherLines; i++) {
    line(
        screen_->cr(),
        Point(
            kDisplayXMid + i * xStep,
            kDisplayRegionYMax),
        Point(
            kDisplayXMid + (i + 1) * xStep,
            kAirballHeight),
        kCowCatcherStroke);
    line(
        screen_->cr(),
        Point(
            kDisplayXMid - i * xStep,
            kDisplayRegionYMax),
        Point(
            kDisplayXMid - (i + 1) * xStep,
            kAirballHeight),
        kCowCatcherStroke);
  }
  line(
      screen_->cr(),
      Point(
          kDisplayXMid - (kNumCowCatcherLines - 1) * xStep,
          kDisplayRegionYMax),
      Point(
          kDisplayXMid + (kNumCowCatcherLines - 1) * xStep,
          kDisplayRegionYMax),
      kCowCatcherStroke);
}

void Display::paintVsi() {
  double radians_per_fpm = (M_PI / 2.0) / kVsiMaxFpm;
  double vsi_width = kVsiHeight / 2 / tan(kVsiPrecisionFpm * radians_per_fpm);
  double vsi_x_left = (kWidth - vsi_width) / 2;
  Point top_left(
      vsi_x_left,
      kAirballHeight + kDisplayMargin);
  Point top_right(
      vsi_x_left + vsi_width,
      top_left.y());
  Point bottom_left(
      top_left.x(),
      top_left.y() + kVsiHeight);
  Point bottom_right(
      top_right.x(),
      bottom_left.y());
  Point center_left(
      top_left.x(),
      top_left.y() + kVsiHeight / 2);
  Point center_right(
      top_right.x(),
      center_left.y());
  rectangle(
      screen_->cr(),
      top_left,
      Size(
          vsi_width,
          kVsiHeight),
      kAltimeterBackgroundColor);
  paintVsiTicMarks(
      top_left,
      top_right,
      center_left,
      center_right,
      bottom_left,
      bottom_right,
      radians_per_fpm);
  paintVsiPointer(
      top_left,
      top_right,
      center_left,
      center_right,
      bottom_left,
      bottom_right,
      radians_per_fpm);
  paintAltitude(
      top_left,
      top_right,
      center_left,
      center_right,
      bottom_left,
      bottom_right);
  paintBaroSetting(
      top_left,
      top_right,
      center_left,
      center_right,
      bottom_left,
      bottom_right);
}

void Display::paintVsiTicMarks(
    Point top_left,
    Point top_right,
    Point center_left,
    Point center_right,
    Point bottom_left,
    Point bottom_right,
    double radians_per_fpm) {
  line(
      screen_->cr(),
      top_right,
      Point(
          top_right.x(),
          top_right.y() + kVsiTickLength),
      kVsiTickStrokeThin);
  line(
      screen_->cr(),
      top_right,
      Point(
          top_right.x() - kVsiTickLength,
          top_right.y()),
      kVsiTickStrokeThin);
  line(
      screen_->cr(),
      bottom_right,
      Point(
          bottom_right.x(),
          bottom_right.y() - kVsiTickLength),
      kVsiTickStrokeThin);
  line(
      screen_->cr(),
      bottom_right,
      Point(
          bottom_right.x() - kVsiTickLength,
          bottom_right.y()),
      kVsiTickStrokeThin);
  line(
      screen_->cr(),
      center_right,
      Point(
          center_right.x() - kVsiTickLength,
          center_right.y()),
      kVsiTickStrokeThin);
  for (int i = 0; i < sizeof(kVsiStepsFpm) / sizeof(kVsiStepsFpm[0]); i++) {
    double step_x =
        (center_left.y() - top_left.y()) /
        tan(kVsiStepsFpm[i].fpm * radians_per_fpm);
    Stroke stroke(
        kVsiTickStrokeThin.color(),
        kVsiTickStrokeThin.width() * kVsiStepsFpm[i].thick);
    line(
        screen_->cr(),
        Point(
            step_x,
            top_left.y()),
        Point(
            step_x,
            top_left.y() + kVsiTickLength),
        stroke);
    line(
        screen_->cr(),
        Point(
            step_x,
            bottom_left.y() - kVsiTickLength),
        Point(
            step_x,
            bottom_left.y()),
        stroke);
  }
}

void Display::paintVsiPointer(
    Point top_left,
    Point top_right,
    Point center_left,
    Point center_right,
    Point bottom_left,
    Point bottom_right,
    double radians_per_fpm) {
  double climb_rate =
      airdata_->climb_rate() / kMetersPerFoot * kSecondsPerMinute;
  climb_rate = fmin(climb_rate, kVsiMaxFpm);
  climb_rate = fmax(climb_rate, -kVsiMaxFpm);
  double angle = climb_rate * radians_per_fpm;
  if (fabs(climb_rate) <= kVsiPrecisionFpm) {
    line(
        screen_->cr(),
        center_left,
        Point(
            center_right.x(),
            center_left.y() - (center_right.x() - center_left.x()) * sin(angle)),
        kVsiPointerStroke);
  } else {
    double dx = (center_left.y() - top_left.y()) / tan(angle);
    Point knee(
        center_left.x() + fabs(dx),
        dx < 0 ? bottom_left.y() : top_left.y());
    line(
        screen_->cr(),
        center_left,
        knee,
        kVsiPointerStroke);
    Point a(
        knee.x(),
        dx < 0
            ? knee.y() - kVsiKneeOffset
            : knee.y() + kVsiKneeOffset);
    Point b(
        center_right.x(),
        a.y());
    line(
        screen_->cr(),
        a,
        b,
        kVsiPointerStroke);
  }
}

void Display::paintAltitude(
  Point top_left,
  Point top_right,
  Point center_left,
  Point center_right,
  Point bottom_left,
  Point bottom_right) {
  int altitude = (int) round(airdata_->altitude() / kMetersPerFoot);
  int thousands = abs(altitude) / 1000;
  int last_three_digits = (abs(altitude) - (thousands * 1000)) / 10 * 10;
  Point baseline(
      center_left.x() + (center_right.x() - center_left.x())
                        * kAltimeterBaselineRatio,
      center_left.y());
  char buf[kPrintBufSize];
  // Print the thousands string
  if (thousands == 0) {
    if (altitude < 0) {
      // Print just a negative sign
      snprintf(
          buf,
          kPrintBufSize,
          "-");
    } else {
      // Leave the thousands blank
      snprintf(
          buf,
          kPrintBufSize,
          "");
    }
  } else {
    if (altitude < 0) {
      snprintf(
          buf,
          kPrintBufSize,
          "-%d",
          thousands);
    } else {
      // Leave the thousands blank
      snprintf(
          buf,
          kPrintBufSize,
          "%d",
          thousands);
    }
  }
  text(
      screen_->cr(),
      buf,
      Point(
          baseline.x() - kAltimeterNumberGap,
          baseline.y()),
      TextReferencePoint::CENTER_RIGHT_UPPERCASE,
      kAltimeterFontLarge,
      kAltimeterTextColor);
  snprintf(
      buf,
      kPrintBufSize,
      "%03d",
      last_three_digits);
  text(
      screen_->cr(),
      buf,
      baseline,
      TextReferencePoint::CENTER_LEFT_UPPERCASE,
      kAltimeterFontSmall,
      kAltimeterTextColor);
}

void Display::paintBaroSetting(
    Point top_left,
    Point top_right,
    Point center_left,
    Point center_right,
    Point bottom_left,
    Point bottom_right) {
  Point baseline(
      center_left.x() + kBaroLeftOffset,
      center_left.y());
  char buf[kPrintBufSize];
  snprintf(
      buf,
      kPrintBufSize,
      "%04.2f",
      settings_->baro_setting());
  text(
      screen_->cr(),
      buf,
      baseline,
      TextReferencePoint::CENTER_LEFT_UPPERCASE,
      kBaroFontSmall,
      kBaroTextColor);
}

void Display::paintNoFlightData() {
  line(
      screen_->cr(),
      Point(0, 0),
      Point(kWidth, kDisplayRegionYMax),
      kNoFlightDataStroke);
  line(
      screen_->cr(),
      Point(kWidth, 0),
      Point(0, kDisplayRegionYMax),
      kNoFlightDataStroke);
}

void Display::paintBatteryStatus() {
  if (!status_->flight_data_up())
    return;

  Point top_left(
      kWidth - kStatusRegionMargin - 2 * kStatusDisplayUnit,
      kStatusRegionMargin);
  Point top_left_inside(
      top_left.x() + kStatusDisplayStrokeWidth/2,
      top_left.y() + kStatusDisplayStrokeWidth/2);
  Size size(
      2 * kStatusDisplayUnit,
      kStatusDisplayUnit);
  rectangle(
      screen_->cr(),
      top_left,
      size,
      kBackground);
  const double barHeight =
      kStatusDisplayUnit - kStatusDisplayStrokeWidth/2;
  const double barWidth =
      status_->battery_health() *
      (2.0 * (kStatusDisplayUnit - kStatusDisplayStrokeWidth/2));;
  const Color barColor =
      status_->battery_health() < 0.5
      ? status_->battery_health() < 0.25
        ? kBatteryColorBad
        : kBatteryColorWarning
      : kBatteryColorGood;
  rectangle(
      screen_->cr(),
      top_left_inside,
      Size(barWidth, barHeight),
      barColor);
  box(
      screen_->cr(),
      top_left,
      size,
      kStatusDisplayStroke);

  if (status_->battery_charging()) {
    text(
        screen_->cr(),
        "⚡️⚡️⚡️",
        Point(top_left_inside.x()+2, top_left_inside.y()),
        TextReferencePoint::TOP_LEFT,
        Font("Noto Sans", 15),
        Color(255, 255, 0));
  }

  if (kStatusDisplayNumericalData || status_->battery_charging()) {
    Point top_left_below(
        top_left.x(),
        top_left.y() + kStatusDisplayUnit + kStatusDisplayStrokeWidth);
    const int buf_size = 20;
    char buf[buf_size];
    snprintf(
        buf,
        buf_size,
        "%.2fV",
        status_->battery_voltage());
    text(
        screen_->cr(),
        buf,
        top_left_below,
        TextReferencePoint::TOP_LEFT,
        kStatusTextFont,
        kStatusTextColor);
    snprintf(
        buf,
        buf_size,
        "%.0fmA",
        status_->battery_current());
    text(
        screen_->cr(),
        buf,
        Point(top_left_below.x(), top_left_below.y()+15),
        TextReferencePoint ::TOP_LEFT,
        kStatusTextFont,
        kStatusTextColor);
  }
}

void Display::paintLinkStatus() {
  if (!status_->flight_data_up())
    return;

  Point bottom_left(
      kWidth - 2 * kStatusRegionMargin - 4 * kStatusDisplayUnit,
      kStatusRegionMargin + kStatusDisplayUnit);
  Point top_right(
      kWidth - 2 * kStatusRegionMargin - 2 * kStatusDisplayUnit,
      kStatusRegionMargin);
  Point bottom_right(
      top_right.x(),
      bottom_left.y());
  Point corners[]{
      bottom_left,
      top_right,
      bottom_right,
  };
  Point top_right_fill(
      bottom_left.x() +
          status_->link_quality() * (top_right.x() - bottom_left.x()),
      bottom_left.y() +
          status_->link_quality() * (top_right.y() - bottom_left.y()));
  Point bottom_right_fill(
      top_right_fill.x(),
      bottom_right.y());
  Point corners_fill[]{
      bottom_left,
      top_right_fill,
      bottom_right_fill,
  };
  shape(
      screen_->cr(),
      3,
      corners_fill,
      kLinkColor);
  polygon(
      screen_->cr(),
      3,
      corners,
      kStatusDisplayStroke);
}

} // namespace airball
