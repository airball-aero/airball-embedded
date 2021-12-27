#include "display.h"

#include <math.h>

#include "units.h"
#include "widgets.h"

namespace airball {

constexpr double kMetersPerFoot = 0.3048;
constexpr double kSecondsPerMinute = 60;

constexpr double ce_floor(double x) {
  return static_cast<double>(static_cast<int64_t>(x));
}

Display::Display(Screen* screen,
                 const Airdata* airdata,
                 const Settings* settings,
                 const SystemStatus* status)
    : screen_(screen), airdata_(airdata), settings_(settings), status_(status)
{}

void Display::layout() {
  fontName_ = "Noto Sans";

  width_ = settings_->screen_width();
  height_ = settings_->screen_height();

  if (settings_->show_altimeter()) {
    altimeterHeight_ = 48;
    airballHeight_ = height_ - altimeterHeight_;
  } else {
    altimeterHeight_ = 0;
    airballHeight_ = height_;
  }

  displayMargin_ = 3;

  topBottomRegionRatio_ = 0.075;

  displayXMid_ = ce_floor(width_ / 2.0);

  displayRegionYMin_ = ce_floor(airballHeight_ * topBottomRegionRatio_);
  displayRegionYMax_ = ce_floor(airballHeight_ * (1.0 - topBottomRegionRatio_));
  displayRegionHeight_ = displayRegionYMax_ - displayRegionYMin_;

  displayRegionWidth_ = width_;
  displayRegionHalfWidth_ = displayRegionWidth_ / 2;

  speedLimitsRosetteHalfAngle_  = (15.0 / 2 / 180 * M_PI);
  trueAirspeedRosetteHalfAngle_ = (70.0 / 2 / 180 * M_PI);

  alphaRefRadius_ = 20;

  alphaRefGapDegrees_ = 40;

  lowSpeedThresholdAirballRadius_ = 0.1 * (width_ / 2.0);

  alphaRefTopAngle0_ = M_PI + (alphaRefGapDegrees_ / 2 / 180 * M_PI);
  alphaRefTopAngle1_ =      - (alphaRefGapDegrees_ / 2 / 180 * M_PI);

  alphaRefBotAngle0_ =        (alphaRefGapDegrees_ / 2 / 180 * M_PI);
  alphaRefBotAngle1_ = M_PI - (alphaRefGapDegrees_ / 2 / 180 * M_PI);

  totemPoleAlphaUnit_ = 20;

  numCowCatcherLines_ = 3;

  background_= Color(0, 0, 0);

  airballFill_ = Color(255, 255, 255);

  rawAirballsMaxBrightness_ = 0.375;

  airballCrosshairsStroke_ = Stroke(
      Color(128, 128, 128),
      2);

  lowSpeedAirballStrokeWidth_ = 4.0;

  tasRingColor_ = Color(255, 0, 255);

  tasRingStrokeWidth_ = 3.0;

  tasThresholdRatio_ = 0.25;

  lowSpeedAirballStroke_ = Stroke(
      Color(255, 255, 255),
      lowSpeedAirballStrokeWidth_);

  lowSpeedAirballArcRadius_ =
      lowSpeedThresholdAirballRadius_ - lowSpeedAirballStrokeWidth_ / 2.0;

  totemPoleStroke_ = Stroke(
      Color(255, 255, 0),
      3);

  cowCatcherStroke_ = Stroke(
      Color(255, 0, 0),
      3);

  vfeStroke_ = Stroke(
      Color(255, 255, 255),
      3);

  vnoStroke_ = Stroke(
      Color(255, 255, 0),
      3);

  vneStroke_ = Stroke(
      Color(255, 0, 0),
      3);

  vBackgroundStroke_ = Stroke(
      Color(0, 0, 0),
      6);

  iASTextFontSize_ =
      width_ / 5.0;

  iASTextFont_ = Font(
      fontName_.c_str(),
      iASTextFontSize_);

  iASTextMargin_ =
      2;

  iASTextColor_ = Color(0, 0, 0);

  printBufSize_ = 128;

  noFlightDataStroke_ = Stroke(
      Color(255, 0, 0),
      3);

  statusRegionMargin_ = 5;

  statusDisplayUnit_ = 20;

  statusDisplayStrokeWidth_ = 2;

  statusDisplayStroke_ = Stroke(
      Color(128, 128, 128),
      statusDisplayStrokeWidth_);

  statusTextFontSize_ = 12;

  statusTextFont_ = Font(
      fontName_.c_str(),
      statusTextFontSize_);

  statusTextColor_ = Color(200, 200, 200);

  batteryColorGood_ = Color(0, 180, 0);

  batteryColorWarning_ = Color(255, 255, 0);

  batteryColorBad_ = Color(255, 0, 0);

  statusDisplayNumericalData_ = false;

  linkColor_ = Color(0, 180, 180);

  vsiHeight_ = altimeterHeight_ - 2 * displayMargin_;

  vsiPrecisionFpm_ = 100;
  vsiMaxFpm_ = 2000;

  vsiStepsFpm_.clear();
  vsiStepsFpm_.push_back(
      {
          .fpm = 200,
          .thick = 1,
      });
  vsiStepsFpm_.push_back(
      {
      .fpm = 300,
      .thick = 1,
      });
  vsiStepsFpm_.push_back(      {
      .fpm = 400,
      .thick = 1,
      });
  vsiStepsFpm_.push_back({
      .fpm = 500,
      .thick = 1.5,
      });
  vsiStepsFpm_.push_back(      {
      .fpm = 1000,
      .thick = 2,
      });
  vsiStepsFpm_.push_back(      {
      .fpm = 1500,
      .thick = 1.5,
      });

  vsiTickLength_ = 7;
  vsiKneeOffset_ = 3;
  vsiTickStrokeThin_ = Stroke(
      Color(255, 255, 255),
      2);
  vsiPointerStroke_ = Stroke(
      Color(255, 0, 255),
      4);
  altimeterFontLarge_ = Font(
      fontName_.c_str(),
      width_ / 8);
  altimeterFontSmall_ = Font(
      fontName_.c_str(),
      width_ / 12);
  altimeterTextColor_= Color(255, 255, 255);
  altimeterBackgroundColor_ = Color(64, 64, 64);
  altimeterBaselineRatio_ = 0.75;
  altimeterNumberGap_ = 7;

  baroLeftOffset_ =
      width_ / 12;
  baroFontSmall_ = Font(
      fontName_.c_str(),
      width_ / 20);
  baroTextColor_ = Color(255, 255, 255);
}

double Display::alpha_to_y(const double alpha) {
  return alpha_degrees_to_y(radians_to_degrees(alpha));
}

double Display::alpha_degrees_to_y(const double alpha_degrees) {
  double ratio = (alpha_degrees - settings_->alpha_min())
                 / (settings_->alpha_stall() - settings_->alpha_min());
  return displayRegionYMin_ + ratio * displayRegionHeight_;
}

double Display::beta_to_x(const double beta) {
  return beta_degrees_to_x(radians_to_degrees(beta));
}

double Display::beta_degrees_to_x(const double beta_degrees) {
  double ratio = (beta_degrees + settings_->beta_bias()) / settings_->beta_full_scale();
  return displayRegionHalfWidth_ * (1.0 + ratio);
}

double Display::airspeed_to_radius(const double airspeed) {
  return airspeed_knots_to_radius(meters_per_second_to_knots(airspeed));
}

double Display::airspeed_knots_to_radius(const double airspeed_nots_) {
  double ratio = airspeed_nots_ / settings_->v_full_scale();
  return ratio * width_ / 2;
}

void Display::paint() {
  layout();

  cairo_push_group(screen_->cr());
  cairo_translate(screen_->cr(), 0, width_);
  cairo_rotate(screen_->cr(), -M_PI / 2);

  paintBackground();

  cairo_save(screen_->cr());
  cairo_rectangle(screen_->cr(), 0, 0, width_, airballHeight_);
  cairo_clip(screen_->cr());
  if (status_->flight_data_up()) {
    paintRawAirballs();
    paintSmoothAirball();
  } else {
    paintNoFlightData();
  }
  paintTotemPole();
  paintCowCatcher();
  if (settings_->show_probe_battery_status()) {
    paintBatteryStatus();
  }
  if (settings_->show_link_status()) {
    paintLinkStatus();
  }
  cairo_restore(screen_->cr());

  if (settings_->show_altimeter()) {
    paintVsi();
  }

  cairo_pop_group_to_source(screen_->cr());
  cairo_paint(screen_->cr());
  cairo_surface_flush(screen_->cs());
}

void Display::paintBackground() {
  rectangle(
      screen_->cr(),
      Point(0, 0),
      Size(width_, height_),
      background_);
}

void Display::paintRawAirballs() {
  for (uint i = airdata_->raw_balls().size(); i-- > 0; ) {
    uint bright_index = airdata_->raw_balls().size() - i;
    double bright =
        ((double) bright_index) / ((double) airdata_->raw_balls().size()) *
        rawAirballsMaxBrightness_;
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
      airballFill_.with_brightness(bright));
}

void Display::paintSmoothAirball() {
  Point center(beta_to_x(airdata_->smooth_ball().beta()), alpha_to_y((airdata_->smooth_ball().alpha())));
  double radius = airspeed_to_radius(airdata_->smooth_ball().ias());
  if (radius < lowSpeedThresholdAirballRadius_) {
    paintAirballLowAirspeed(center);
  } else {
    paintAirballAirspeed(center, radius);
    if ((radius * 2) > (iASTextFontSize_ * 1.5)) {
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
      lowSpeedAirballArcRadius_,
      0,
      2.0 * M_PI,
      lowSpeedAirballStroke_);
}

void Display::paintAirballAirspeed(const Point& center, const double radius) {
  disc(
      screen_->cr(),
      center,
      radius,
      airballFill_);
  line(
      screen_->cr(),
      Point(center.x(), center.y() - radius),
      Point(center.x(), center.y() + radius),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(center.x() - radius, center.y()),
      Point(center.x() + radius, center.y()),
      airballCrosshairsStroke_);
}

void Display::paintAirballAirspeedText(const Point& center, const double ias) {
  char buf[printBufSize_];
  double ias_nots_ = meters_per_second_to_knots(ias);
  snprintf(
      buf,
      printBufSize_,
      "%.0f",
      ias_nots_);
  Size sz = text_size(screen_->cr(), buf, iASTextFont_);
  rectangle(
      screen_->cr(),
      Point(
          center.x() - sz.w() / 2 - iASTextMargin_,
          center.y() - sz.h() / 2 - iASTextMargin_),
      Size(
          sz.w() + 2 * iASTextMargin_,
          sz.h() + 2 * iASTextMargin_),
      airballFill_);
  text(
      screen_->cr(),
      buf,
      center,
      TextReferencePoint::CENTER_MID_UPPERCASE,
      iASTextFont_,
      iASTextColor_);
}

void Display::paintAirballAirspeedLimits(const Point& center) {
  if (meters_per_second_to_knots(airdata_->smooth_ball().ias()) < settings_->v_r()) {
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
          center.x() - r - totemPoleAlphaUnit_,
          center.y() + totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x() - r,
          center.y()),
      Point(
          center.x() - r - totemPoleAlphaUnit_,
          center.y() - totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x() + r,
          center.y()),
      Point(
          center.x() + r + totemPoleAlphaUnit_,
          center.y() + totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x() + r,
          center.y()),
      Point(
          center.x() + r + totemPoleAlphaUnit_,
          center.y() - totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() + r),
      Point(
          center.x() + totemPoleAlphaUnit_,
          center.y() + r + totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() + r),
      Point(
          center.x() - totemPoleAlphaUnit_,
          center.y() + r + totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() - r),
      Point(
          center.x() + totemPoleAlphaUnit_,
          center.y() - r - totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() - r),
      Point(
          center.x() - totemPoleAlphaUnit_,
          center.y() - r - totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
}

void Display::paintAirballAirspeedLimitsNormal(const Point& center) {
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_fe()),
      4,
      speedLimitsRosetteHalfAngle_,
      M_PI_4,
      vBackgroundStroke_);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_fe()),
      4,
      speedLimitsRosetteHalfAngle_,
      M_PI_4,
      vfeStroke_);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_no()),
      4,
      speedLimitsRosetteHalfAngle_,
      M_PI_4,
      vBackgroundStroke_);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_no()),
      4,
      speedLimitsRosetteHalfAngle_,
      M_PI_4,
      vnoStroke_);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_ne()),
      4,
      speedLimitsRosetteHalfAngle_,
      M_PI_4,
      vBackgroundStroke_);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_ne()),
      4,
      speedLimitsRosetteHalfAngle_,
      M_PI_4,
      vneStroke_);
}

void Display::paintAirballTrueAirspeed(const Point& center) {
  double tas_stroe_alpha_ = 0;
  if (airdata_->smooth_ball().tas() <
      airdata_->smooth_ball().ias() || airdata_->smooth_ball().ias() == 0) {
    tas_stroe_alpha_ = 0;
  } else {
    double ias_squared =
        airdata_->smooth_ball().ias() * airdata_->smooth_ball().ias();
    double tas_squared =
        airdata_->smooth_ball().tas() * airdata_->smooth_ball().tas();
    double ratio = (tas_squared - ias_squared) / ias_squared;
    tas_stroe_alpha_ = (ratio > tasThresholdRatio_)
                       ? 1.0 : (ratio / tasThresholdRatio_);
  }
  rosette(
      screen_->cr(),
      center,
      airspeed_to_radius(airdata_->smooth_ball().tas()),
      4,
      trueAirspeedRosetteHalfAngle_,
      0,
      Stroke(
          tasRingColor_.with_alpha(tas_stroe_alpha_),
          tasRingStrokeWidth_));
}

void Display::paintTotemPole() {
  paintTotemPoleLine();
  paintTotemPoleAlphaX();
  paintTotemPoleAlphaY();
}

void Display::paintTotemPoleLine() {
  line(
      screen_->cr(),
      Point(displayXMid_, 0),
      Point(displayXMid_, alpha_degrees_to_y(settings_->alpha_ref()) - alphaRefRadius_),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(displayXMid_, alpha_degrees_to_y(settings_->alpha_ref()) + alphaRefRadius_),
      Point(displayXMid_, displayRegionYMax_),
      totemPoleStroke_);
  arc(
      screen_->cr(),
      Point(displayXMid_, alpha_degrees_to_y(settings_->alpha_ref())),
      alphaRefRadius_,
      alphaRefTopAngle0_,
      alphaRefTopAngle1_,
      totemPoleStroke_);
  arc(
      screen_->cr(),
      Point(displayXMid_, alpha_degrees_to_y(settings_->alpha_ref())),
      alphaRefRadius_,
      alphaRefBotAngle0_,
      alphaRefBotAngle1_,
      totemPoleStroke_);
}

void Display::paintTotemPoleAlphaX() {
  line(
      screen_->cr(),
      Point(
          displayXMid_ - 3 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_x())),
      Point(
          displayXMid_ - 2 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_x())),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ - 2 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_x())),
      Point(
          displayXMid_ - 3 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_x()) - totemPoleAlphaUnit_) ,
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ + 3 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_x())),
      Point(
          displayXMid_ + 2 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_x())),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ + 2 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_x())),
      Point(
          displayXMid_ + 3 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_x()) - totemPoleAlphaUnit_) ,
      totemPoleStroke_);
}

void Display::paintTotemPoleAlphaY() {
  line(
      screen_->cr(),
      Point(
          displayXMid_ - 4 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_y())),
      Point(
          displayXMid_ - 5 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_y())),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ - 5 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_y())),
      Point(
          displayXMid_ - 6 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_y()) - totemPoleAlphaUnit_),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ + 4 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_y())),
      Point(
          displayXMid_ + 5 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_y())),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ + 5 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_y())),
      Point(
          displayXMid_ + 6 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(settings_->alpha_y()) - totemPoleAlphaUnit_),
      totemPoleStroke_);
}

void Display::paintCowCatcher() {
  double xStep =
      (displayRegionWidth_ - (2 * displayMargin_)) /
      (2 * numCowCatcherLines_);
  for (int i = 0; i < numCowCatcherLines_; i++) {
    line(
        screen_->cr(),
        Point(
            displayXMid_ + i * xStep,
            displayRegionYMax_),
        Point(
            displayXMid_ + (i + 1) * xStep,
            airballHeight_),
        cowCatcherStroke_);
    line(
        screen_->cr(),
        Point(
            displayXMid_ - i * xStep,
            displayRegionYMax_),
        Point(
            displayXMid_ - (i + 1) * xStep,
            airballHeight_),
        cowCatcherStroke_);
  }
  line(
      screen_->cr(),
      Point(
          displayXMid_ - (numCowCatcherLines_ - 1) * xStep,
          displayRegionYMax_),
      Point(
          displayXMid_ + (numCowCatcherLines_ - 1) * xStep,
          displayRegionYMax_),
      cowCatcherStroke_);
}

void Display::paintVsi() {
  double radians_per_fpm = (M_PI / 2.0) / vsiMaxFpm_;
  double vsi_width = vsiHeight_ / 2 / tan(vsiPrecisionFpm_ * radians_per_fpm);
  double vsi_x_left = (width_ - vsi_width) / 2;
  Point top_left(
      vsi_x_left,
      airballHeight_ + displayMargin_);
  Point top_right(
      vsi_x_left + vsi_width,
      top_left.y());
  Point bottom_left(
      top_left.x(),
      top_left.y() + vsiHeight_);
  Point bottom_right(
      top_right.x(),
      bottom_left.y());
  Point center_left(
      top_left.x(),
      top_left.y() + vsiHeight_ / 2);
  Point center_right(
      top_right.x(),
      center_left.y());
  rectangle(
      screen_->cr(),
      top_left,
      Size(
          vsi_width,
          vsiHeight_),
      altimeterBackgroundColor_);
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
          top_right.y() + vsiTickLength_),
      vsiTickStrokeThin_);
  line(
      screen_->cr(),
      top_right,
      Point(
          top_right.x() - vsiTickLength_,
          top_right.y()),
      vsiTickStrokeThin_);
  line(
      screen_->cr(),
      bottom_right,
      Point(
          bottom_right.x(),
          bottom_right.y() - vsiTickLength_),
      vsiTickStrokeThin_);
  line(
      screen_->cr(),
      bottom_right,
      Point(
          bottom_right.x() - vsiTickLength_,
          bottom_right.y()),
      vsiTickStrokeThin_);
  line(
      screen_->cr(),
      center_right,
      Point(
          center_right.x() - vsiTickLength_,
          center_right.y()),
      vsiTickStrokeThin_);
  for (auto i = vsiStepsFpm_.begin(); i < vsiStepsFpm_.end(); ++i) {
    double step_x =
        (center_left.y() - top_left.y()) /
        tan(i->fpm * radians_per_fpm);
    Stroke stroke(
        vsiTickStrokeThin_.color(),
        vsiTickStrokeThin_.width() * i->thick);
    line(
        screen_->cr(),
        Point(
            step_x,
            top_left.y()),
        Point(
            step_x,
            top_left.y() + vsiTickLength_),
        stroke);
    line(
        screen_->cr(),
        Point(
            step_x,
            bottom_left.y() - vsiTickLength_),
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
  climb_rate = fmin(climb_rate, vsiMaxFpm_);
  climb_rate = fmax(climb_rate, -vsiMaxFpm_);
  double angle = climb_rate * radians_per_fpm;
  if (fabs(climb_rate) <= vsiPrecisionFpm_) {
    line(
        screen_->cr(),
        center_left,
        Point(
            center_right.x(),
            center_left.y() - (center_right.x() - center_left.x()) * sin(angle)),
        vsiPointerStroke_);
  } else {
    double dx = (center_left.y() - top_left.y()) / tan(angle);
    Point nee_(
        center_left.x() + fabs(dx),
        dx < 0 ? bottom_left.y() : top_left.y());
    line(
        screen_->cr(),
        center_left,
        nee_,
        vsiPointerStroke_);
    Point a(
        nee_.x(),
        dx < 0
            ? nee_.y() - vsiKneeOffset_
            : nee_.y() + vsiKneeOffset_);
    Point b(
        center_right.x(),
        a.y());
    line(
        screen_->cr(),
        a,
        b,
        vsiPointerStroke_);
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
                        * altimeterBaselineRatio_,
      center_left.y());
  char buf[printBufSize_];
  // Print the thousands string
  if (thousands == 0) {
    if (altitude < 0) {
      // Print just a negative sign
      snprintf(
          buf,
          printBufSize_,
          "-");
    } else {
      // Leave the thousands blank
      snprintf(
          buf,
          printBufSize_,
          "");
    }
  } else {
    if (altitude < 0) {
      snprintf(
          buf,
          printBufSize_,
          "-%d",
          thousands);
    } else {
      // Leave the thousands blank
      snprintf(
          buf,
          printBufSize_,
          "%d",
          thousands);
    }
  }
  text(
      screen_->cr(),
      buf,
      Point(
          baseline.x() - altimeterNumberGap_,
          baseline.y()),
      TextReferencePoint::CENTER_RIGHT_UPPERCASE,
      altimeterFontLarge_,
      altimeterTextColor_);
  snprintf(
      buf,
      printBufSize_,
      "%03d",
      last_three_digits);
  text(
      screen_->cr(),
      buf,
      baseline,
      TextReferencePoint::CENTER_LEFT_UPPERCASE,
      altimeterFontSmall_,
      altimeterTextColor_);
}

void Display::paintBaroSetting(
    Point top_left,
    Point top_right,
    Point center_left,
    Point center_right,
    Point bottom_left,
    Point bottom_right) {
  Point baseline(
      center_left.x() + baroLeftOffset_,
      center_left.y());
  char buf[printBufSize_];
  snprintf(
      buf,
      printBufSize_,
      "%04.2f",
      settings_->baro_setting());
  text(
      screen_->cr(),
      buf,
      baseline,
      TextReferencePoint::CENTER_LEFT_UPPERCASE,
      baroFontSmall_,
      baroTextColor_);
}

void Display::paintNoFlightData() {
  line(
      screen_->cr(),
      Point(0, 0),
      Point(width_, displayRegionYMax_),
      noFlightDataStroke_);
  line(
      screen_->cr(),
      Point(width_, 0),
      Point(0, displayRegionYMax_),
      noFlightDataStroke_);
}

void Display::paintBatteryStatus() {
  if (!status_->flight_data_up())
    return;

  Point top_left(
      width_ - statusRegionMargin_ - 2 * statusDisplayUnit_,
      statusRegionMargin_);
  Point top_left_inside(
      top_left.x() + statusDisplayStrokeWidth_/2,
      top_left.y() + statusDisplayStrokeWidth_/2);
  Size size(
      2 * statusDisplayUnit_,
      statusDisplayUnit_);
  rectangle(
      screen_->cr(),
      top_left,
      size,
      background_);
  const double barHeight =
      statusDisplayUnit_ - statusDisplayStrokeWidth_/2;
  const double barWidth =
      status_->battery_health() *
      (2.0 * (statusDisplayUnit_ - statusDisplayStrokeWidth_/2));;
  const Color barColor =
      status_->battery_health() < 0.5
      ? status_->battery_health() < 0.25
        ? batteryColorBad_
        : batteryColorWarning_
      : batteryColorGood_;
  rectangle(
      screen_->cr(),
      top_left_inside,
      Size(barWidth, barHeight),
      barColor);
  box(
      screen_->cr(),
      top_left,
      size,
      statusDisplayStroke_);

  if (status_->battery_charging()) {
    text(
        screen_->cr(),
        "⚡️⚡️⚡️",
        Point(top_left_inside.x()+2, top_left_inside.y()),
        TextReferencePoint::TOP_LEFT,
        Font("Noto Sans", 15),
        Color(255, 255, 0));
  }

  if (statusDisplayNumericalData_ || status_->battery_charging()) {
    Point top_left_below(
        top_left.x(),
        top_left.y() + statusDisplayUnit_ + statusDisplayStrokeWidth_);
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
        statusTextFont_,
        statusTextColor_);
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
        statusTextFont_,
        statusTextColor_);
  }
}

void Display::paintLinkStatus() {
  if (!status_->flight_data_up())
    return;

  Point bottom_left(
      width_ - 2 * statusRegionMargin_ - 4 * statusDisplayUnit_,
      statusRegionMargin_ + statusDisplayUnit_);
  Point top_right(
      width_ - 2 * statusRegionMargin_ - 2 * statusDisplayUnit_,
      statusRegionMargin_);
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
      linkColor_);
  polygon(
      screen_->cr(),
      3,
      corners,
      statusDisplayStroke_);
}

} // namespace airball
