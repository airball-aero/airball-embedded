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

#ifndef AIRBALL_SETTINGS_H
#define AIRBALL_SETTINGS_H

#include <string>
#include <map>

namespace airball {

struct Parameter;

class Settings {
public:
  Settings(std::string path);
  ~Settings() {}

  /**
   * @return the display setting, full scale indicated air speed. (Knots)
   */
  double v_full_scale() const;

  /**
   * @return the rotation indicated air speed (Vr). (Knots)
   */
  double v_r() const;

  /**
   * @return the maximum flap extension indicated air speed (Vfe). (Knots)
   */
  double v_fe() const;

  /**
   * @return the maximum normal operating indicated air speed (Vno). (Knots)
   */
  double v_no() const;

  /**
   * @return the never-exceed indicated air speed (Vne). (Knots)
   */
  double v_ne() const;

  /**
   * @return the stall angle of attack. (Degrees)
   */
  double alpha_stall() const;

  /**
   * @return the display setting, minimum angle of attack shown. (Degrees)
   */
  double alpha_min() const;

  /**
   * @return the maximum climb rate angle of attack. (Degrees)
   */
  double alpha_x() const;

  /**
   * @return the maximum climb angle angle of attack. (Degrees)
   */
  double alpha_y() const;

  /**
   * @return the reference stabilized approach angle of attack. (Degrees)
   */
  double alpha_ref() const;

  /**
   * @return the display setting, full scale angle of yaw. (Degrees)
   */
  double beta_full_scale() const;

  /**
   * @return the display setting, constant bias to angle of yaw. (Degrees)
   */
  double beta_bias() const;

  /**
   * @return the current barometer setting. (Inches of mercury)
   */
  double baro_setting() const;

  /**
   * @return whether the settings are currently being adjusted.
   */
  bool adjusting() const;

  /**
   * @return the name of the settings parameter currently being ajusted,
   *     or the empty string if !adjusting().
   */
  std::string adjusting_param_name() const;

  /**
   * @return the current value of the settings parameter currently being
   *     adjusted, or an undefined value if !adjusting().
   */
  double adjusting_param_value() const;

  /**
   * Commands this Settings to start or stop adjusting.
   *
   * @param adjusting whether to be in the adjusting state.
   */
  void set_adjusting(bool adjusting);

  /*
   * Commands this Settings to advance to adjust the next settings parameter.
   */
  void adjust_next();

  /**
   * Commands this Settings to adjust the current parameter one step up.
   */
  void adjust_up();

  /**
   * Commands this Settings to adjust the current parameter one step down.
   */
  void adjust_down();

  /**
   * Commands this Settings to adjust the 'baro_setting' parameter (which is
   * not part of the usual parameter sequence) one step up.
   */
  void adjust_baro_setting_up();

  /**
   * Commands this Settings to adjust the 'baro_setting' parameter (which is
   * not part of the usual parameter sequence) one step down.
   */
  void adjust_baro_setting_down();

private:

  void load();
  void save();

  void load_str(std::string);
  std::string save_str();

  double get_value(const Parameter* p) const;
  void adjust_param_up(const Parameter* p);
  void adjust_param_down(const Parameter* p);

    std::string path_;
  std::map<const std::string, long int> values_;
  int adjusting_param_index_;
};

} // namespace airball

#endif // AIRBALL_SETTINGS_H
