#include "sound_scheme.h"

namespace airball {

sound_scheme::sound_scheme(
    std::string device_name,
    const ISettings* settings,
    const ILocalParams* local_params,
    const IAirdata* airdata)
    : mixer_(device_name),
      settings_(settings),
      local_params_(local_params),
      airdata_(airdata) {
}

bool sound_scheme::start() {
  update();
  return mixer_.start();
}

} // namespace airball