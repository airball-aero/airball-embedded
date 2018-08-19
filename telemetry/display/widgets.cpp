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

#include "widgets.h"

#include <math.h>
#include <cairo/cairo.h>
#include <iostream>

namespace airball {

void Color::apply(cairo_t *cr) const {
  cairo_set_source_rgba(cr, r_, g_, b_, a_);
}

void Stroke::apply(cairo_t *cr) const {
  cairo_set_line_width(cr, width_);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  color_.apply(cr);
}

void Font::apply(cairo_t *cr) const {
  cairo_select_font_face(
      cr,
      face_,
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, size_);
}

void line(
    cairo_t* cr,
    const Point& start,
    const Point& end,
    const Stroke& stroke) {
  cairo_move_to(cr, start.x(), start.y());
  cairo_line_to(cr, end.x(), end.y());
  stroke.apply(cr);
  cairo_stroke(cr);
}

void arc(
    cairo_t* cr,
    const Point& center,
    const double radius,
    const double start_angle,
    const double end_angle,
    const Stroke& stroke) {
  cairo_arc(cr,
            center.x(),
            center.y(),
            radius,
            start_angle,
            end_angle);
  stroke.apply(cr);
  cairo_stroke(cr);
}

void disc(
    cairo_t* cr,
    const Point& center,
    const double radius,
    const Color& fill) {
  cairo_arc(cr,
            center.x(),
            center.y(),
            radius,
            0,
            2 * M_PI);
  fill.apply(cr);
  cairo_fill(cr);
}

void rosette(
    cairo_t* cr,
    const Point& center,
    const double radius,
    const int num_petals,
    const double petal_half_angle,
    const double start_angle,
    const Stroke& stroke) {
  double angle_increment = 2 * M_PI / ((double) num_petals);
  for (int i = 0; i < num_petals; i++) {
    double angle = start_angle + i * angle_increment;
    arc(cr,
        center,
        radius,
        angle - petal_half_angle,
        angle + petal_half_angle,
        stroke);
  }
}

void rectangle(
    cairo_t* cr,
    const Point& top_left,
    const Size& size,
    const Color& fill) {
  cairo_rectangle(
      cr,
      top_left.x(),
      top_left.y(),
      size.w(),
      size.h());
  fill.apply(cr);
  cairo_fill(cr);
}

void box(
    cairo_t* cr,
    const Point& top_left,
    const Size& size,
    const Stroke& stroke) {
  cairo_rectangle(
      cr,
      top_left.x(),
      top_left.y(),
      size.w(),
      size.h());
  stroke.apply(cr);
  cairo_stroke(cr);
}

void polygon(
    cairo_t* cr,
    const int numCorners,
    const Point corners[],
    const Stroke& stroke) {
  cairo_move_to(
      cr,
      corners[0].x(),
      corners[0].y());
  for (int i = 1; i < numCorners; i++) {
    cairo_line_to(
        cr,
        corners[i].x(),
        corners[i].y());
  }
  cairo_close_path(cr);
  stroke.apply(cr);
  cairo_stroke(cr);
}

void shape(
    cairo_t* cr,
    const int numCorners,
    const Point corners[],
    const Color& fill) {
  cairo_move_to(
      cr,
      corners[0].x(),
      corners[0].y());
  for (int i = 1; i < numCorners; i++) {
    cairo_line_to(
        cr,
        corners[i].x(),
        corners[i].y());
  }
  cairo_close_path(cr);
  fill.apply(cr);
  cairo_fill(cr);
}

void text_top_left(
    cairo_t* cr,
    const std::string& str,
    const Point& top_left,
    const Font& font,
    const Color& color) {
  font.apply(cr);
  color.apply(cr);
  cairo_move_to(
      cr,
      top_left.x(),
      top_left.y() + font.size());
  cairo_show_text(cr, str.c_str());
}

void text_top_right(
    cairo_t* cr,
    const std::string& str,
    const Point& top_right,
    const Font& font,
    const Color& color) {
  font.apply(cr);
  color.apply(cr);
  cairo_text_extents_t extents;
  cairo_text_extents (cr, str.c_str(), &extents);
  cairo_move_to(
      cr,
      top_right.x() - extents.width,
      top_right.y() + font.size());
  cairo_show_text(cr, str.c_str());
}

} // namespace airball