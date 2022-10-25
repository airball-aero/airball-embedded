#ifndef AIRBALL_SETTINGS_H
#define AIRBALL_SETTINGS_H

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "isettings.h"

namespace airball {

template <class T> class Parameter;

class Settings : public ISettings {
public:
  Settings();
  ~Settings() {}

  double v_full_scale() const override;
  double v_r() const override;
  double v_fe() const override;
  double v_no() const override;
  double v_ne() const override;
  double alpha_stall() const override;
  double alpha_stall_warning() const override;
  double alpha_min() const override;
  double alpha_max() const override;
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
  bool declutter() const override;
  std::string sound_scheme() const override;
  double audio_volume() const override;
  std::string speed_units() const override;
  bool rotate_screen() const override;
  
  /**
   * Load settings from a specified file path.
   */
   void load(const std::string &path);

private:

  void load_str(std::string);

  template <class T> T get_value(const Parameter<T>* p) const;

  rapidjson::Document document_;
};

} // namespace airball

#endif // AIRBALL_SETTINGS_H
