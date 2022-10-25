#ifndef BCM2835_SMI_IOCTL_DEFS_H
#define BCM2835_SMI_IOCTL_DEFS_H

namespace airball {

class bcm2835_smi_ioctl_defs {
public:

  static unsigned long bcm2835_smi_ioc_get_settings();
  static unsigned long bcm2835_smi_ioc_write_settings();

  static void* settings_allocate();
  static void settings_apply(void* settings);
  static void settings_free(void* settings);
};
  
}  // namespace airball

#endif // BCM2835_SMI_IOCTL_DEFS_H
