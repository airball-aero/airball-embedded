#ifndef AIRBALL_SETTINGS_H
#define AIRBALL_SETTINGS_H

#include "isettings.h"
#include "parameter_file.h"

namespace airball {

class Settings : public ISettings {
public:
  explicit Settings(const std::string& path);
  ~Settings() = default;

  double v_full_scale() const override;
  double v_r() const override;
  double v_fe() const override;
  double v_no() const override;
  double v_ne() const override;
  double alpha_stall() const override;
  double alpha_stall_warning() const override;
  double alpha_min() const override;
  double alpha_x() const override;
  double alpha_y() const override;
  double alpha_ref() const override;
  double beta_full_scale() const override;
  double beta_bias() const override;
  double baro_setting() const override;
  double ball_smoothing_factor() const override;
  double vsi_smoothing_factor() const override;
  int screen_width() const override;
  int screen_height() const override;
  bool show_altimeter() const override;
  bool show_link_status() const override;
  bool show_probe_battery_status() const override;
  std::string sound_scheme() const override;
  double audio_volume() const override;

  void load();

private:
  ParameterFile file_;
};

} // namespace airball

#endif // AIRBALL_SETTINGS_H
