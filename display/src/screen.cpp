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

#include "screen.h"

#include <cairo/cairo-xlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>

namespace airball {

class X11Screen : public Screen {
public:
  X11Screen(const int x, const int y);

  ~X11Screen();

  int next_event() const;

  virtual cairo_t* cr() const override { return cr_; }

  virtual cairo_surface_t* cs() const override { return cs_; }

private:
  cairo_t *cr_;
  cairo_surface_t *cs_;
};

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
      0, 0, x, y, 0, 0, 0);
  XSelectInput(dsp, da, ButtonPressMask | KeyPressMask);
  XMapWindow(dsp, da);

  cs_ = cairo_xlib_surface_create(
      dsp, da,
      DefaultVisual(dsp, screen), x, y);
  cairo_xlib_surface_set_size(cs_, x, y);

  cr_ = cairo_create(cs_);
}

X11Screen::~X11Screen() {
  cairo_destroy(cr_);
  Display *dsp = cairo_xlib_surface_get_display(cs_);
  cairo_surface_destroy(cs_);
  XCloseDisplay(dsp);
}

int X11Screen::next_event() const {
  char keybuf[8];
  KeySym key;
  XEvent e;

  XNextEvent(cairo_xlib_surface_get_display(cs_), &e);

  switch (e.type) {
    case ButtonPress:
      printf("============== Got ButtonPress event\n");
      return -e.xbutton.button;
    case KeyPress:
      XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
      printf("============== Got event %s\n", keybuf);
      return key;
    default:
      fprintf(stderr, "Dropping unhandled XEevent.type = %d.\n", e.type);
      return key;
  }
}

class FramebufferScreen : public Screen {
public:
  FramebufferScreen();

  ~FramebufferScreen();

  int next_event() const { return 0; }

  virtual cairo_t* cr() const override { return cr_; }

  virtual cairo_surface_t* cs() const override { return cs_; }

private:
  void setUpFb();

  void tearDownFb();

  int fbfd_;
  unsigned char *fbp_;
  int xres_;
  int yres_;
  int bits_per_pixel_;
  long int screensize_;
  cairo_surface_t *cs_;
  cairo_t *cr_;
};

FramebufferScreen::FramebufferScreen() {
  setUpFb();
  cs_ = cairo_image_surface_create_for_data(
      fbp_,
      CAIRO_FORMAT_ARGB32,
      xres_,
      yres_,
      cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, xres_));
  cr_ = cairo_create(cs_);
}

FramebufferScreen::~FramebufferScreen() {
  cairo_destroy(cr_);
  cairo_surface_destroy(cs_);
  tearDownFb();
}

void FramebufferScreen::setUpFb() {
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;

  // Open the file for reading and writing
  fbfd_ = open("/dev/fb0", O_RDWR);
  if (fbfd_ == -1) {
    perror("Error: cannot open framebuffer device");
    exit(1);
  }

  // Get fixed screen information
  if (ioctl(fbfd_, FBIOGET_FSCREENINFO, &finfo) == -1) {
    perror("Error reading fixed information");
    exit(2);
  }

  // Get variable screen information
  if (ioctl(fbfd_, FBIOGET_VSCREENINFO, &vinfo) == -1) {
    perror("Error reading variable information");
    exit(3);
  }

  screensize_ = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

  // Map the device to memory
  fbp_ = (unsigned char *) mmap(0, screensize_, PROT_READ | PROT_WRITE,
                                MAP_SHARED, fbfd_, 0);
  if ((size_t) fbp_ == -1) {
    perror("Error: failed to map framebuffer device to memory");
    exit(4);
  }
  xres_ = vinfo.xres;
  yres_ = vinfo.yres;
  bits_per_pixel_ = vinfo.bits_per_pixel;
}

void FramebufferScreen::tearDownFb() {
  munmap(fbp_, screensize_);
  close(fbfd_);
}

Screen* Screen::NewX11Screen(const int x, const int y) {
  return new X11Screen(x, y);
}

Screen* Screen::NewFramebufferScreen() {
  return new FramebufferScreen();
}

}  // namespace airball