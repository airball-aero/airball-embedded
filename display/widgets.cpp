#include "widgets.h"

#include <math.h>
#include <cairo/cairo.h>

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