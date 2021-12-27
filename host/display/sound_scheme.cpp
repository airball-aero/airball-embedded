#include "sound_scheme.h"

namespace airball {

sound_scheme::sound_scheme(
    std::string device_name,
    ISettings* settings,
    IAirdata* airdata)
    : mixer_(device_name), settings_(settings), airdata_(airdata) {
}

bool sound_scheme::start() {
  update();
  return mixer_.start();
}

} // namespace airball