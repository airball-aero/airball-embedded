#ifndef AIRBALL_DISPLAY_SOUND_SCHEME_H
#define AIRBALL_DISPLAY_SOUND_SCHEME_H

#include <string>
#include <vector>
#include "sound_layer.h"
#include "sound_mixer.h"
#include "isettings.h"
#include "ilocalparams.h"
#include "iairdata.h"

namespace airball {

class sound_scheme {
public:
  explicit sound_scheme(std::string device_name,
                        const ISettings* settings,
                        const ILocalParams* local_params,
                        const IAirdata *airdata);
  virtual ~sound_scheme() = default;

  bool start();

  virtual void update() = 0;

protected:
  sound_mixer& mixer() { return mixer_; }

  const ISettings& settings() { return *settings_; }

  const ILocalParams& local_params() { return *local_params_; }

  const IAirdata& airdata() { return *airdata_; }

private:
  sound_mixer mixer_;
  const ISettings* const settings_;
  const ILocalParams* const local_params_;
  const IAirdata* const airdata_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_SOUND_SCHEME_H
