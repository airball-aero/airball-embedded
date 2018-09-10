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

#include "display.h"

#include <math.h>
#include <iostream>

#include "units.h"
#include "widgets.h"

namespace airball {

///////////////////////////////////////////////////////////////////////

constexpr double kWidth = 272;
constexpr double kHeight = 480;

constexpr double kTopBottomRegionRatio = 0.10;

constexpr double kDisplayXMid = floor(kWidth / 2.0);

constexpr double kDisplayRegionYMin = floor(kHeight * kTopBottomRegionRatio);
constexpr double kDisplayRegionYMax = floor(kHeight * (1.0 - kTopBottomRegionRatio));
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

constexpr Color kAirballFill(0, 0, 255);

constexpr Stroke kAirballCrosshairsStroke(
    Color(255, 255, 255),
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

constexpr double kAdjustingTextFontSize = kWidth / 14;
constexpr double kAdjustingRegionWidth = kAdjustingTextFontSize * 10;
constexpr double kAdjustingRegionHeight = kAdjustingTextFontSize + 8;

constexpr Stroke kAdjustingWindowStroke(
    Color(255, 255, 255),
    1);

constexpr char kAdjustingTextFontName[] =
    "Noto Sans";

constexpr Font kAdjustingTextFont(
    kAdjustingTextFontName,
    kAdjustingTextFontSize);

constexpr Color kAdjustingTextColor(0, 255, 0);

constexpr Point kAdjustingTopLeft(kWidth/2 - kAdjustingRegionWidth/2, kHeight-kAdjustingRegionHeight-5);
constexpr Size kAdjustingWindowSize(kAdjustingRegionWidth, kAdjustingRegionHeight);
constexpr Point kAdjustingParamTopLeft(kWidth/2 - kAdjustingRegionWidth/2 + 5, kHeight-kAdjustingRegionHeight-5);
constexpr Point kAdjustingValueTopRight(kWidth/2 + kAdjustingRegionWidth/2 - 5, kHeight-kAdjustingRegionHeight-5);
constexpr int kAdjustingValueBufSize = 128;

constexpr Stroke kNoFlightDataStroke(
    Color(255, 0, 0),
    3);

constexpr double kStatusRegionMargin = 5;

constexpr double kStatusDisplayUnit = 20;

constexpr double kStatusDisplayStrokeWidth = 2;

constexpr Stroke kStatusDisplayStroke(
    Color(128, 128, 128),
    kStatusDisplayStrokeWidth);

constexpr Color kBatteryColorGood(0, 255, 0);

constexpr Color kBatteryColorWarning(255, 255, 0);

constexpr Color kBatteryColorBad(255, 0, 0);

constexpr Color kLinkColor(0, 255, 255);

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
  if (status_->flight_data_up()) {
    paintAirball();
  } else {
    paintNoFlightData();
  }
  paintTotemPole();
  paintCowCatcher();
  paintBatteryStatus();
  paintLinkStatus();
  if (settings_->adjusting()) {
    paintAdjusting();
  }
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

void Display::paintAirball() {
  Point center(beta_to_x(airdata_->beta()), alpha_to_y((airdata_->alpha())));
  double radius = airspeed_to_radius(airdata_->ias());
  if (radius < kLowSpeedThresholdAirballRadius) {
    paintAirballLowAirspeed(center);
  } else {
    paintAirballAirspeed(center, radius);
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
      0,
      kVfeStroke);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_no()),
      4,
      kSpeedLimitsRosetteHalfAngle,
      0,
      kVnoStroke);
  rosette(
      screen_->cr(),
      center,
      airspeed_knots_to_radius(settings_->v_ne()),
      4,
      kSpeedLimitsRosetteHalfAngle,
      0,
      kVneStroke);
}

void Display::paintAirballTrueAirspeed(const Point& center) {
  double tas_stroke_alpha;
  if (airdata_->tas() < airdata_->ias() || airdata_->ias() == 0) {
    tas_stroke_alpha = 0;
  } else {
    double ias_squared = airdata_->ias() * airdata_->ias();
    double tas_squared = airdata_->tas() * airdata_->tas();
    double ratio = (tas_squared - ias_squared) / ias_squared;
    tas_stroke_alpha = (ratio > kTasThresholdRatio)
                       ? 1.0 : (ratio / kTasThresholdRatio);
  }
  rosette(
      screen_->cr(),
      center,
      airspeed_to_radius(airdata_->tas()),
      4,
      kTrueAirspeedRosetteHalfAngle,
      0.25 * M_PI,
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
  double xStep = kDisplayRegionWidth / (2 * kNumCowCatcherLines);
  for (int i = 0; i < kNumCowCatcherLines; i++) {
    line(
        screen_->cr(),
        Point(
            kDisplayXMid + i * xStep,
            kDisplayRegionYMax),
        Point(
            kDisplayXMid + (i + 1) * xStep,
            kHeight),
        kCowCatcherStroke);
    line(
        screen_->cr(),
        Point(
            kDisplayXMid - i * xStep,
            kDisplayRegionYMax),
        Point(
            kDisplayXMid - (i + 1) * xStep,
            kHeight),
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

void Display::paintAdjusting() {
  rectangle(
      screen_->cr(),
      kAdjustingTopLeft,
      kAdjustingWindowSize,
      kBackground);
  box(
      screen_->cr(),
      kAdjustingTopLeft,
      kAdjustingWindowSize,
      kAdjustingWindowStroke);
  text_top_left(
      screen_->cr(),
      settings_->adjusting_param_name(),
      kAdjustingParamTopLeft,
      kAdjustingTextFont,
      kAdjustingTextColor);
  char buf[kAdjustingValueBufSize];
  snprintf(
      buf,
      kAdjustingValueBufSize,
      "%.2f",
      settings_->adjusting_param_value());
  text_top_right(
      screen_->cr(),
      buf,
      kAdjustingValueTopRight,
      kAdjustingTextFont,
      kAdjustingTextColor);
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
      (2.0 * (kStatusDisplayUnit - kStatusDisplayStrokeWidth/2));
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
