#ifndef AIRBALL_DISPLAY_LOCALPARAMS_H
#define AIRBALL_DISPLAY_LOCALPARAMS_H

#include "ilocalparams.h"
#include "parameter_file.h"

namespace airball {

class LocalParams : public ILocalParams {
public:
  LocalParams(const std::string& path);
  virtual ~LocalParams() = default;

  double audio_volume() const override;
  void audio_volume(double value) override;

  double screen_brightness() const override;
  void screen_brightness(double value) override;

  double baro_setting_in_hg() const override;
  void baro_setting_in_hg(double value) override;

private:
  const std::string path_;
  ParameterFile file_;
};

}  // namespace airball

#endif  // AIRBALL_DISPLAY_LOCALPARAMS_H
