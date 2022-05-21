#ifndef AIRBALL_DISPLAY_LOCALPARAMSCONTROLLER_H
#define AIRBALL_DISPLAY_LOCALPARAMSCONTROLLER_H

#include "localparams.h"

namespace airball {

class LocalParamsController {
public:
  LocalParamsController(LocalParams* local_params);
  virtual ~LocalParamsController() = default;

  void click();
  void increment();
  void decrement();

  enum Mode {
    AUDIO_VOLUME,
    SCREEN_BRIGHTNESS,
    BARO_SETTING_IN_HG,
  };

  Mode mode();

private:
  LocalParams* local_params_;
};

}  // namespace airball

#endif  // AIRBALL_DISPLAY_LOCALPARAMSCONTROLLER_H
