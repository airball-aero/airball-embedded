#include "st7789vi_frame_writer_smi.h"

#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "bcm2835_smi_ioctl_defs.h"

#include <pigpio.h>

namespace airball {

st7789vi_frame_writer_smi::~st7789vi_frame_writer_smi() {
  close(fd_);
  fd_ = -1;
}

void st7789vi_frame_writer_smi::initialize() {
  fd_= open("/dev/smi", O_RDWR);
  if (fd_ < 0) fail("cannot open");

  void* settings = bcm2835_smi_ioctl_defs::settings_allocate();

  int ret = ioctl(fd_, bcm2835_smi_ioctl_defs::bcm2835_smi_ioc_get_settings(), settings);
  if (ret != 0) fail("ioctl 1");

  bcm2835_smi_ioctl_defs::settings_apply(settings);

  ret = ioctl(fd_, bcm2835_smi_ioctl_defs::bcm2835_smi_ioc_write_settings(), settings);
  if (ret != 0) fail("ioctl 2");

  bcm2835_smi_ioctl_defs::settings_free(settings);

  st7789vi_frame_writer::initialize();
}

void st7789vi_frame_writer_smi::write_data(uint16_t* buf, int len) {
  ::write(fd_, buf, len * sizeof(uint16_t));
}

}  // namespace airball
