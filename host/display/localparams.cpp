#include "localparams.h"

namespace airball {

const ParameterFile::Parameter<double> AUDIO_VOLUME(
    "audio_volume",
    1.0);

const ParameterFile::Parameter<double> SCREEN_BRIGHTNESS(
    "screen_brightness",
    1.0);

const ParameterFile::Parameter<double> BARO_SETTING_IN_HG(
    "baro_setting_in_hg",
    29.92);

LocalParams::LocalParams(const std::string &path)
    : file_(path) {}

double LocalParams::audio_volume() const {
  return file_.get_value(&AUDIO_VOLUME);
}

void LocalParams::audio_volume(double value) {
  file_.set_value(&AUDIO_VOLUME, value, true);
}

double LocalParams::screen_brightness() const {
  return file_.get_value(&SCREEN_BRIGHTNESS);
}

void LocalParams::screen_brightness(double value) {
  file_.set_value(&SCREEN_BRIGHTNESS, value, true);
}

double LocalParams::baro_setting_in_hg() const {
  return file_.get_value(&BARO_SETTING_IN_HG);
}

void LocalParams::baro_setting_in_hg(double value) {
  file_.set_value(&BARO_SETTING_IN_HG, value, true);
}

}  // namespace airball
