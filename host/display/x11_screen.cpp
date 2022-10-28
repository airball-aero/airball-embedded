#include "x11_screen.h"

#include <cairo/cairo-xlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <math.h>

namespace airball {

X11Screen::X11Screen(const int x, const int y) {
  Display *dsp;
  Drawable da;
  int screen;
  cairo_surface_t *sfc;

  if ((dsp = XOpenDisplay(NULL)) == NULL)
    exit(1);

  screen = DefaultScreen(dsp);
  da = XCreateSimpleWindow(
      dsp, DefaultRootWindow(dsp),
      0, 0, y, x, 0, 0, 0);
  XSelectInput(dsp, da, ButtonPressMask | KeyPressMask);
  XMapWindow(dsp, da);

  set_cs(cairo_xlib_surface_create(
      dsp, da,
      DefaultVisual(dsp, screen), y, x));
  cairo_xlib_surface_set_size(cs(), y, x);

  set_cr(cairo_create(cs()));

  cairo_rotate(cr(), M_PI / 2);
  cairo_translate(cr(), 0, -y);
}

X11Screen::~X11Screen() {
  cairo_destroy(cr());
  Display *dsp = cairo_xlib_surface_get_display(cs());
  cairo_surface_destroy(cs());
  XCloseDisplay(dsp);
}

}  // namespace airball
