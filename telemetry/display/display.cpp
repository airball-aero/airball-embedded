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

constexpr double kWidth = 480;
constexpr double kHeight = 800;

constexpr double kTopBottomRegionRatio = 0.10;

constexpr double kDisplayXMid = floor(kWidth / 2.0);

constexpr double kDisplayRegionYMin = floor(kHeight * kTopBottomRegionRatio);
constexpr double kDisplayRegionYMax = floor(kHeight * (1.0 - kTopBottomRegionRatio));
constexpr double kDisplayRegionHeight = kDisplayRegionYMax - kDisplayRegionYMin;

constexpr double kDisplayRegionWidth = kWidth;
constexpr double kDisplayRegionHalfWidth = kDisplayRegionWidth / 2;

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

constexpr Stroke kAdjustingWindowStroke(
    Color(255, 255, 255),
    1);

constexpr char kAdjustingTextFontName[] =
    "Noto Sans";

constexpr Font kAdjustingTextFont(
    kAdjustingTextFontName,
    30);

constexpr Color kAdjustingTextColor(0, 255, 0);

constexpr Point kAdjustingTopLeft(200, 740);
constexpr Size kAdjustingWindowSize(260, 50);
constexpr Point kAdjustingParamTopLeft(210, 745);
constexpr Point kAdjustingValueTopRight(450, 745);
constexpr int kAdjustingValueBufSize = 128;

constexpr Stroke kNoFlightDataStroke(
    Color(255, 0, 0),
    3);

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
  double airspeed_knots = meters_per_second_to_knots(airspeed);
  double ratio = airspeed_knots / settings_->ias_full_scale();
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
    arc(
        screen_->cr(),
        center,
        kLowSpeedAirballArcRadius,
        0,
        2.0 * M_PI,
        kLowSpeedAirballStroke);
  } else {
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
    arc(
        screen_->cr(),
        center,
        airspeed_to_radius(airdata_->tas()),
        0,
        2.0 * M_PI,
        Stroke(
            kTasRingColor.with_alpha(tas_stroke_alpha),
            kTasRingStrokeWidth));
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

} // namespace airball
