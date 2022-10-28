#include "framebuffer_screen.h"

#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>

namespace airball {

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

  // Set to graphics mode
  ioctl(STDOUT_FILENO, KDSETMODE, KD_GRAPHICS);
  ioctl(STDERR_FILENO, KDSETMODE, KD_GRAPHICS);
}

void FramebufferScreen::tearDownFb() {
  munmap(fbp_, screensize_);
  close(fbfd_);
}

}  // namespace airball
