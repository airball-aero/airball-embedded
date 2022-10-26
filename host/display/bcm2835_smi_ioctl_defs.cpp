#include "bcm2835_smi_ioctl_defs.h"

#include <uapi/asm-generic/int-ll64.h>
#include <linux/broadcom/bcm2835_smi.h>

namespace airball {

unsigned long bcm2835_smi_ioctl_defs::bcm2835_smi_ioc_get_settings() {
  return BCM2835_SMI_IOC_GET_SETTINGS;
}

unsigned long bcm2835_smi_ioctl_defs::bcm2835_smi_ioc_write_settings() {
  return BCM2835_SMI_IOC_WRITE_SETTINGS;  
}    

void* bcm2835_smi_ioctl_defs::settings_allocate() {
  return new smi_settings();
}    

void bcm2835_smi_ioctl_defs::settings_apply(void* settings) {
  auto ss = (struct smi_settings*) settings;
  ss->data_width = SMI_WIDTH_16BIT;
  ss->write_setup_time = 5;
  ss->write_strobe_time = 5;
  ss->write_hold_time = 5;
  ss->write_pace_time = 5;
}

void bcm2835_smi_ioctl_defs::settings_free(void* settings) {
  auto ss = (struct smi_settings*) settings;
  delete ss;
}
  
}  // namespace airball
