#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <math.h>

void flip(unsigned char* fbp, long int screensize) {
  unsigned char b = 0x00;

  while (true) {
    std::cerr << std::hex << " 0x" << b;
    for (int i = 0; i < screensize; i++) {
      fbp[i] = b;
    }
    b = (b > 0 ? 0x00 : 0xff);
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(100));
  }
}

void run_test() {
  int fbfd_;
  unsigned char *fbp_;
  int xres_;
  int yres_;
  int bits_per_pixel_;
  long int screensize_;

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

  // Set to graphics mode
  ioctl(STDOUT_FILENO, KDSETMODE, KD_GRAPHICS);
  ioctl(STDERR_FILENO, KDSETMODE, KD_GRAPHICS);  

  std::cout << "screensize_ = " << screensize_ << std::endl;  
  std::cout << "fbfd_ = " << fbfd_ << std::endl;

  flip(fbp_, screensize_);
  
  munmap(fbp_, screensize_);
  close(fbfd_);
}

int main(int arg, char** argv) {
  run_test();
}
