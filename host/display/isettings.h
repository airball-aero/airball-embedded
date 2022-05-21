#ifndef AIRBALL_DISPLAY_ISETTINGS_H
#define AIRBALL_DISPLAY_ISETTINGS_H

#include <string>

namespace airball {

class ISettings {
public:
  /**
   * @return the display setting, full scale indicated air speed. (Knots)
   */
  virtual double v_full_scale() const = 0;

  /**
   * @return the rotation indicated air speed (Vr). (Knots)
   */
  virtual double v_r() const = 0;

  /**
   * @return the maximum flap extension indicated air speed (Vfe). (Knots)
   */
  virtual double v_fe() const = 0;

  /**
   * @return the maximum normal operating indicated air speed (Vno). (Knots)
   */
  virtual double v_no() const = 0;

  /**
   * @return the never-exceed indicated air speed (Vne). (Knots)
   */
  virtual double v_ne() const = 0;

  /**
   * @return the stall angle of attack. (Degrees)
   */
  virtual double alpha_stall() const = 0;

  /**
   * @return the stall warning angle of attack. (Degrees)
   */
  virtual double alpha_stall_warning() const = 0;

  /**
   * @return the display setting, minimum angle of attack shown. (Degrees)
   */
  virtual double alpha_min() const = 0;

  /**
   * @return the maximum climb rate angle of attack. (Degrees)
   */
  virtual double alpha_x() const = 0;

  /**
   * @return the maximum climb angle angle of attack. (Degrees)
   */
  virtual double alpha_y() const = 0;

  /**
   * @return the reference stabilized approach angle of attack. (Degrees)
   */
  virtual double alpha_ref() const = 0;

  /**
   * @return  full scale angle of yaw. (Degrees)
   */
  virtual double beta_full_scale() const = 0;

  /**
   * @return the display setting, constant bias to angle of yaw. (Degrees)
   */
  virtual double beta_bias() const = 0;

  /**
   * @return the current barometer setting. (Inches of mercury)
   */
  virtual double baro_setting() const = 0;

  /**
   * @return the ball smoothing factor.
   */
  virtual double ball_smoothing_factor() const = 0;

  /**
   * @return the VSI smoothing factor.
   */
  virtual double vsi_smoothing_factor() const = 0;

  /**
   * @return the screen width.
   */
  virtual int screen_width() const = 0;

  /**
   * @return the screen height.
   */
  virtual int screen_height() const = 0;

  /**
   * @return whether to display the altimeter.
   */
  virtual bool show_altimeter() const = 0;

  /**
   * @return whether to display the wireless link status.
   */
  virtual bool show_link_status() const = 0;

  /**
   * @return whether to display the probe battery status.
   */
  virtual bool show_probe_battery_status() const = 0;

  /**
   * @return the name of the sound scheme to use.
   */
  virtual std::string sound_scheme() const = 0;

  /**
   * @return the desired audio volume, from 0.0 (none) to 1.0 (full).
   */
  virtual double audio_volume() const = 0;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_ISETTINGS_H
