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
  constexpr Color(double r, double g, double b) : r_(r), g_(g), b_(b), a_(1) {}
  constexpr Color(double r, double g, double b, double a) : r_(r), g_(g), b_(b), a_(a) {}
  constexpr Color(int r, int g, int b)
      : Color(hexToRatio(r), hexToRatio(g), hexToRatio(b)) {}
  constexpr Color with_alpha(double a) const {
    return Color(r_, g_, b_, a);
  }
  double r() const {return r_;}
  double g() const {return g_;}
  double b() const {return b_;}
  double a() const {return a_;}
  void apply(cairo_t *cr) const;
private:
  static constexpr double hexToRatio(int hex) {
    return (double) hex / (double) 0xff;
  }
  const double r_, g_, b_, a_;
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

void polygon(
    cairo_t* cr,
    const int numCorners,
    const Point corners[],
    const Stroke& stroke);

void shape(
    cairo_t* cr,
    const int numCorners,
    const Point corners[],
    const Color& fill);

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
