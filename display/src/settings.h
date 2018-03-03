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
   * @return the display setting, full scale indicated air speed.
   */
  double ias_full_scale() const;

  /**
   * @return the stall angle of attack.
   */
  double alpha_stall() const;

  /**
   * @return the display setting, minimum angle of attack shown.
   */
  double alpha_min() const;

  /**
   * @return the maximum climb rate angle of attack.
   */
  double alpha_x() const;

  /**
   * @return the maximum climb angle angle of attack.
   */
  double alpha_y() const;

  /**
   * @return the reference stabilized approach angle of attack.
   */
  double alpha_ref() const;

  /**
   * @return the display setting, full scale angle of yaw.
   */
  double beta_full_scale() const;

  /**
   * @return the display setting, constant bias to angle of yaw.
   */
  double beta_bias() const;

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

private:

  void load();
  void save();

  void load_str(std::string);
  std::string save_str();

  double get_value(const Parameter* parameter) const;

  std::string path_;
  std::map<const std::string, long int> values_;
  int adjusting_param_index_;
};

} // namespace airball

#endif // AIRBALL_SETTINGS_H
