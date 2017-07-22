#ifndef AIRBALL_WIDGETS_H
#define AIRBALL_WIDGETS_H

#include <string>
#include <vector>
#include <cairo/cairo.h>

namespace airball {

// Basic classes

class Point {
public:
  constexpr Point(double x, double y) : x_(x), y_(y) {}
  double x() const {return x_;}
  double y() const {return y_;}
private:
  const double x_, y_;
};

class Size {
public:
  constexpr Size(double w, double h) : w_(w), h_(h) {}
  double w() const {return w_;}
  double h() const {return h_;}
private:
  const double w_, h_;
};

class Color {
public:
  constexpr Color(double r, double g, double b) : r_(r), g_(g), b_(b) {}
  constexpr Color(int r, int g, int b)
      : Color(hexToRatio(r), hexToRatio(g), hexToRatio(b)) {}
  double r() const {return r_;}
  double g() const {return g_;}
  double b() const {return b_;}
  void apply(cairo_t *cr) const;
private:
  static constexpr double hexToRatio(int hex) {
    return (double) hex / (double) 0xff;
  }
  const double r_, g_, b_;
};

class Stroke {
public:
  constexpr Stroke(Color color, double width) : color_(color), width_(width) {}
  Color color() const {return color_;}
  double width() const {return width_;}
  void apply(cairo_t *cr) const;
private:
  const Color color_;
  const double width_;
};

class Font {
public:
  constexpr Font(
      const char* face,
      const double size)
  : face_(face), size_(size) {}
  const char* face() const { return face_; }
  double size() const { return size_; }
  void apply(cairo_t* cr) const;
private:
  const char* face_;
  const double size_;
};

void line(
    cairo_t* cr,
    const Point& start,
    const Point& end,
    const Stroke& stroke);

void arc(
    cairo_t* cr,
    const Point& center,
    const double radius,
    const double start_angle,
    const double end_angle,
    const Stroke& stroke);

void disc(
    cairo_t* cr,
    const Point& center,
    const double radius,
    const Color& fill);

void rectangle(
    cairo_t* cr,
    const Point& top_left,
    const Size& size,
    const Color& fill);

void box(
    cairo_t* cr,
    const Point& top_left,
    const Size& size,
    const Stroke& stroke);

void text_top_left(
    cairo_t* cr,
    const std::string& str,
    const Point& top_left,
    const Font& font,
    const Color& color);

void text_top_right(
    cairo_t* cr,
    const std::string& str,
    const Point& top_right,
    const Font& font,
    const Color& color);

} // namespace airball

#endif // AIRBALL_WIDGETS_H
