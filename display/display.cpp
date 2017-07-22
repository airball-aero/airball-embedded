#include "display.h"

#include <math.h>

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

///////////////////////////////////////////////////////////////////////

double Display::alphaToY(const double alpha) {
  double ratio = (alpha - settings_->alpha_min()) / (settings_->alpha_stall() - settings_->alpha_min());
  return kDisplayRegionYMin + ratio * kDisplayRegionHeight;
}

double Display::betaToX(const double beta) {
  double ratio = beta / settings_->beta_full_scale();
  return kDisplayRegionHalfWidth * (1.0 + ratio);
}

double Display::iasToRadius(const double ias) {
  double ratio = ias / settings_->ias_full_scale();
  return ratio * kWidth / 2;
}

void Display::paint() {
  cairo_push_group(screen_->cr());
  cairo_translate(screen_->cr(), 0, kWidth);
  cairo_rotate(screen_->cr(), -M_PI / 2);

  paintBackground();
  paintAirball();
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
  Point center(betaToX(airdata_->beta()), alphaToY((airdata_->alpha())));
  double radius = iasToRadius(airdata_->ias());
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

void Display::paintTotemPole() {
  paintTotemPoleLine();
  paintTotemPoleAlphaX();
  paintTotemPoleAlphaY();
}

void Display::paintTotemPoleLine() {
  line(
      screen_->cr(),
      Point(kDisplayXMid, 0),
      Point(kDisplayXMid, alphaToY(settings_->alpha_ref()) - kAlphaRefRadius),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(kDisplayXMid, alphaToY(settings_->alpha_ref()) + kAlphaRefRadius),
      Point(kDisplayXMid, kDisplayRegionYMax),
      kTotemPoleStroke);
  arc(
      screen_->cr(),
      Point(kDisplayXMid, alphaToY(settings_->alpha_ref())),
      kAlphaRefRadius,
      kAlphaRefTopAngle0,
      kAlphaRefTopAngle1,
      kTotemPoleStroke);
  arc(
      screen_->cr(),
      Point(kDisplayXMid, alphaToY(settings_->alpha_ref())),
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
          alphaToY(settings_->alpha_x())),
      Point(
          kDisplayXMid - 2 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_x())),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid - 2 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_x())),
      Point(
          kDisplayXMid - 3 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_x()) - kTotemPoleAlphaUnit) ,
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid + 3 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_x())),
      Point(
          kDisplayXMid + 2 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_x())),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid + 2 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_x())),
      Point(
          kDisplayXMid + 3 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_x()) - kTotemPoleAlphaUnit) ,
      kTotemPoleStroke);
}

void Display::paintTotemPoleAlphaY() {
  line(
      screen_->cr(),
      Point(
          kDisplayXMid - 4 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_y())),
      Point(
          kDisplayXMid - 5 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_y())),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid - 5 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_y())),
      Point(
          kDisplayXMid - 6 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_y()) - kTotemPoleAlphaUnit),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid + 4 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_y())),
      Point(
          kDisplayXMid + 5 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_y())),
      kTotemPoleStroke);
  line(
      screen_->cr(),
      Point(
          kDisplayXMid + 5 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_y())),
      Point(
          kDisplayXMid + 6 * kTotemPoleAlphaUnit,
          alphaToY(settings_->alpha_y()) - kTotemPoleAlphaUnit),
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

} // namespace airball
