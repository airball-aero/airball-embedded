#include "settings.h"

namespace airball {

const ParameterFile::Parameter<double> V_FULL_SCALE(
    "ias_full_scale",
    100);

const ParameterFile::Parameter<double> V_R(
    "v_r",
    100);

const ParameterFile::Parameter<double> V_FE(
    "v_fe",
    100);

const ParameterFile::Parameter<double> V_NO(
    "v_no",
    100);

const ParameterFile::Parameter<double> V_NE(
    "v_ne",
    100);

const ParameterFile::Parameter<double> ALPHA_STALL(
    "alpha_stall",
    15.0);

const ParameterFile::Parameter<double> ALPHA_STALL_WARNING(
    "alpha_stall_warning",
    14.0);

const ParameterFile::Parameter<double> ALPHA_MIN(
    "alpha_min",
    0.0);

const ParameterFile::Parameter<double> ALPHA_X(
    "alpha_x",
    12.0);

const ParameterFile::Parameter<double> ALPHA_Y(
    "alpha_y",
    10.0);

const ParameterFile::Parameter<double> ALPHA_REF(
    "alpha_ref",
    14.0);

const ParameterFile::Parameter<double> BETA_FULL_SCALE(
    "beta_full_scale",
    20.0);

const ParameterFile::Parameter<double> BETA_BIAS(
    "beta_bias",
    0.0);

const ParameterFile::Parameter<double> BARO_SETTING(
    "baro_setting",
    29.92);

const ParameterFile::Parameter<double> BALL_SMOOTHING_FACTOR(
    "ball_smoothing_factor",
    1.0);

const ParameterFile::Parameter<double> VSI_SMOOTHING_FACTOR(
    "vsi_smoothing_factor",
    1.0);

const ParameterFile::Parameter<int> SCREEN_WIDTH(
    "screen_width",
    272);

const ParameterFile::Parameter<int> SCREEN_HEIGHT(
    "screen_height",
    480);

const ParameterFile::Parameter<bool> SHOW_ALTIMETER(
    "show_altimeter",
    true);

const ParameterFile::Parameter<bool> SHOW_LINK_STATUS(
    "show_link_status",
    true);

const ParameterFile::Parameter<bool> SHOW_PROBE_BATTERY_STATUS(
    "show_probe_battery_status",
    true);

const ParameterFile::Parameter<std::string> SOUND_SCHEME(
    "sound_scheme",
    "flyonspeed");

const ParameterFile::Parameter<double> AUDIO_VOLUME(
    "audio_volume",
    1.0);

Settings::Settings(const std::string& path)
    : file_(path) {}

void Settings::load() {
  file_.load();
}

double Settings::v_full_scale() const {
  return file_.get_value(&V_FULL_SCALE);
}

double Settings::v_r() const {
  return file_.get_value(&V_R);
}

double Settings::v_fe() const {
  return file_.get_value(&V_FE);
}

double Settings::v_no() const {
  return file_.get_value(&V_NO);
}

double Settings::v_ne() const {
  return file_.get_value(&V_NE);
}

double Settings::alpha_stall() const {
  return file_.get_value(&ALPHA_STALL);
}

double Settings::alpha_stall_warning() const {
  return file_.get_value(&ALPHA_STALL_WARNING);
}

double Settings::alpha_min() const {
  return file_.get_value(&ALPHA_MIN);
}

double Settings::alpha_x() const {
  return file_.get_value(&ALPHA_X);
}

double Settings::alpha_y() const {
  return file_.get_value(&ALPHA_Y);
}

double Settings::alpha_ref() const {
  return file_.get_value(&ALPHA_REF);
}

double Settings::beta_full_scale() const {
  return file_.get_value(&BETA_FULL_SCALE);
}

double Settings::beta_bias() const {
  return file_.get_value(&BETA_BIAS);
}

double Settings::baro_setting() const {
  return file_.get_value(&BARO_SETTING);
}

double Settings::ball_smoothing_factor() const {
  return file_.get_value(&BALL_SMOOTHING_FACTOR);
}

double Settings::vsi_smoothing_factor() const {
  return file_.get_value(&VSI_SMOOTHING_FACTOR);
}

int Settings::screen_width() const {
  return file_.get_value(&SCREEN_WIDTH);
}

int Settings::screen_height() const {
  return file_.get_value(&SCREEN_HEIGHT);
}

bool Settings::show_altimeter() const {
  return file_.get_value(&SHOW_ALTIMETER);
}

bool Settings::show_link_status() const {
  return file_.get_value(&SHOW_LINK_STATUS);
}

bool Settings::show_probe_battery_status() const {
  return file_.get_value(&SHOW_PROBE_BATTERY_STATUS);
}

std::string Settings::sound_scheme() const {
  return file_.get_value(&SOUND_SCHEME);
}

double Settings::audio_volume() const {
  return file_.get_value(&AUDIO_VOLUME);
}

} // namespace airball

