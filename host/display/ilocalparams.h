#ifndef AIRBALL_DISPLAY_ILOCALPARAMS_H
#define AIRBALL_DISPLAY_ILOCALPARAMS_H

namespace airball {

class ILocalParams {
public:
  virtual double audio_volume() const = 0;
  virtual void audio_volume(double value) = 0;

  virtual double screen_brightness() const = 0;
  virtual void screen_brightness(double value) = 0;

  virtual double baro_setting_in_hg() const = 0;
  virtual void baro_setting_in_hg(double value) = 0;
};

}  // namespace airball

#endif  // AIRBALL_DISPLAY_ILOCALPARAMS_H
