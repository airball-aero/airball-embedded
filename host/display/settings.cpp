#include "settings.h"

#include <iostream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <fstream>
#include <sstream>

namespace airball {

struct Parameter {
  const char *name;
  const char *display_name;
  const char *display_units;
  const double min;
  const double max;
  const double initial;
};

constexpr const char* ANGULAR_PARAMETER_DISPLAY_UNITS = "°";
constexpr double ANGULAR_PARAMETER_ABSMAX = 45.0;

constexpr const char* V_PARAMETER_DISPLAY_UNITS = "kt";
constexpr double V_PARAMETER_MIN = 0;
constexpr double V_PARAMETER_MAX = 500;
constexpr double V_PARAMETER_INITIAL = 100;

constexpr Parameter V_FULL_SCALE = {
    .name="ias_full_scale",
    .display_name = "IAS FS",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .min=V_PARAMETER_MIN,
    .max=V_PARAMETER_MAX,
    .initial=V_PARAMETER_INITIAL,
};

constexpr Parameter V_R = {
    .name="v_r",
    .display_name = "Vr",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .min=V_PARAMETER_MIN,
    .max=V_PARAMETER_MAX,
    .initial=V_PARAMETER_INITIAL,
};

constexpr Parameter V_FE = {
    .name="v_fe",
    .display_name = "Vfe",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .min=V_PARAMETER_MIN,
    .max=V_PARAMETER_MAX,
    .initial=V_PARAMETER_INITIAL,
};

constexpr Parameter V_NO = {
    .name="v_no",
    .display_name = "Vno",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .min=V_PARAMETER_MIN,
    .max=V_PARAMETER_MAX,
    .initial=V_PARAMETER_INITIAL,
};

constexpr Parameter V_NE = {
    .name="v_ne",
    .display_name = "Vne",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .min=V_PARAMETER_MIN,
    .max=V_PARAMETER_MAX,
    .initial=V_PARAMETER_INITIAL,
};

constexpr Parameter ALPHA_STALL = {
    .name="alpha_stall",
    .display_name = "α stall",
    .display_units = ANGULAR_PARAMETER_DISPLAY_UNITS,
    .min=-ANGULAR_PARAMETER_ABSMAX,
    .max=ANGULAR_PARAMETER_ABSMAX,
    .initial=15.0,
};

constexpr Parameter ALPHA_MIN = {
    .name="alpha_min",
    .display_name = "α min",
    .display_units = ANGULAR_PARAMETER_DISPLAY_UNITS,
    .min=-ANGULAR_PARAMETER_ABSMAX,
    .max=ANGULAR_PARAMETER_ABSMAX,
    .initial=0.0,
};

constexpr Parameter ALPHA_X = {
    .name="alpha_x",
    .display_name = "α X",
    .display_units = ANGULAR_PARAMETER_DISPLAY_UNITS,
    .min=-ANGULAR_PARAMETER_ABSMAX,
    .max=ANGULAR_PARAMETER_ABSMAX,
    .initial=12.0,
};

constexpr Parameter ALPHA_Y = {
    .name="alpha_y",
    .display_name = "α Y",
    .display_units = ANGULAR_PARAMETER_DISPLAY_UNITS,
    .min=-ANGULAR_PARAMETER_ABSMAX,
    .max=ANGULAR_PARAMETER_ABSMAX,
    .initial=10.0,
};

constexpr Parameter ALPHA_REF = {
    .name="alpha_ref",
    .display_name = "α ref",
    .display_units = ANGULAR_PARAMETER_DISPLAY_UNITS,
    .min=-ANGULAR_PARAMETER_ABSMAX,
    .max=ANGULAR_PARAMETER_ABSMAX,
    .initial=14.0,
};

constexpr Parameter BETA_FULL_SCALE = {
    .name="beta_full_scale",
    .display_name = "β max",
    .display_units = ANGULAR_PARAMETER_DISPLAY_UNITS,
    .min=0.0,
    .max=ANGULAR_PARAMETER_ABSMAX,
    .initial=20.0,
};

constexpr Parameter BETA_BIAS = {
    .name="beta_bias",
    .display_name = "β bias",
    .display_units = ANGULAR_PARAMETER_DISPLAY_UNITS,
    .min=-ANGULAR_PARAMETER_ABSMAX,
    .max=ANGULAR_PARAMETER_ABSMAX,
    .initial=0.0,
};

constexpr Parameter BARO_SETTING = {
    .name="baro_setting",
    .display_name = "baro",
    "in Hg",
    .min=27.92,
    .max=31.92,
    .initial=29.92,
};

constexpr Parameter BALL_SMOOTHING_FACTOR = {
    .name="ball_smoothing_factor",
    .display_name = "SF ball",
    "",
    .min=0.0,
    .max=1.0,
    .initial=1.0,
};

constexpr Parameter VSI_SMOOTHING_FACTOR = {
    .name="vsi_smoothing_factor",
    .display_name = "SF vsi",
    "",
    .min=0.0,
    .max=1.0,
    .initial=1.0,
};

constexpr const Parameter* ALL_SETTINGS[]{
    &V_FULL_SCALE,
    &V_R,
    &V_FE,
    &V_NO,
    &V_NE,
    &ALPHA_STALL,
    &ALPHA_MIN,
    &ALPHA_X,
    &ALPHA_Y,
    &ALPHA_REF,
    &BETA_FULL_SCALE,
    &BETA_BIAS,
    &BARO_SETTING,
    &BALL_SMOOTHING_FACTOR,
    &VSI_SMOOTHING_FACTOR,
};

constexpr const int NUM_ALL_SETTINGS =
    sizeof(ALL_SETTINGS) / sizeof(Parameter*);

Settings::Settings() {
  for (int i = 0; i < NUM_ALL_SETTINGS; i++) {
    values_[ALL_SETTINGS[i]->name] = ALL_SETTINGS[i]->initial;
  }
}

void Settings::load_str(std::string str) {
  std::cout << "load_str(" << str << ")" << std::endl;
  rapidjson::Document document;
  document.Parse("{}");
  document.Parse(str.c_str());
  for (int i = 0; i < NUM_ALL_SETTINGS; i++) {
    const Parameter* p = ALL_SETTINGS[i];
    if (document.HasMember(p->name)) {
      double value = document[p->name].GetDouble();
      std::cout << " value = " << value << std::endl;
      if (!(value < p->min || value > p->max)) {
        values_[p->name] = value;
      }
    }
  }
}

void Settings::load(const std::string& path) {
  std::cout << "load(" << path << ")" << std::endl;
  std::ifstream f;
  f.open(path);
  std::stringstream s;
  s << f.rdbuf();
  f.close();
  load_str(s.str());
}

double Settings::get_value(const Parameter* p) const {
  return values_.at(p->name);
}

double Settings::v_full_scale() const {
  return get_value(&V_FULL_SCALE);
}

double Settings::v_r() const {
  return get_value(&V_R);
}

double Settings::v_fe() const {
  return get_value(&V_FE);
}

double Settings::v_no() const {
  return get_value(&V_NO);
}

double Settings::v_ne() const {
  return get_value(&V_NE);
}

double Settings::alpha_stall() const {
  return get_value(&ALPHA_STALL);
}

double Settings::alpha_min() const {
  return get_value(&ALPHA_MIN);
}

double Settings::alpha_x() const {
  return get_value(&ALPHA_X);
}

double Settings::alpha_y() const {
  return get_value(&ALPHA_Y);
}

double Settings::alpha_ref() const {
  return get_value(&ALPHA_REF);
}

double Settings::beta_full_scale() const {
  return get_value(&BETA_FULL_SCALE);
}

double Settings::beta_bias() const {
  return get_value(&BETA_BIAS);
}

double Settings::baro_setting() const {
  return get_value(&BARO_SETTING);
}

double Settings::ball_smoothing_factor() const {
  return get_value(&BALL_SMOOTHING_FACTOR);
}

double Settings::vsi_smoothing_factor() const {
  return get_value(&VSI_SMOOTHING_FACTOR);
}

} // namespace airball

