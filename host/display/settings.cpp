#include "settings.h"

#include <iostream>
#include <rapidjson/document.h>
#include <fstream>
#include <sstream>

namespace airball {

template<class T>
class Parameter {
public:
  const char *name;
  const T initial;

  T get(const rapidjson::Document &doc) const {
    if (doc.HasMember(name)) {
      return get_impl(doc);
    }
    return initial;
  }

  T get_impl(const rapidjson::Document &doc) const;
};

template<>
int Parameter<int>::get_impl(const rapidjson::Document &doc) const {
  return doc[name].GetInt();
}

template<>
double Parameter<double>::get_impl(const rapidjson::Document &doc) const {
  return doc[name].GetDouble();
}

template<>
bool Parameter<bool>::get_impl(const rapidjson::Document &doc) const {
  return doc[name].GetBool();
}

constexpr Parameter<double> V_FULL_SCALE = {
    .name="ias_full_scale",
    .initial=100,
};

constexpr Parameter<double> V_R = {
    .name="v_r",
    .initial=100,
};

constexpr Parameter<double> V_FE = {
    .name="v_fe",
    .initial=100,
};

constexpr Parameter<double> V_NO = {
    .name="v_no",
    .initial=100,
};

constexpr Parameter<double> V_NE = {
    .name="v_ne",
    .initial=100,
};

constexpr Parameter<double> ALPHA_STALL = {
    .name="alpha_stall",
    .initial=15.0,
};

constexpr Parameter<double> ALPHA_MIN = {
    .name="alpha_min",
    .initial=0.0,
};

constexpr Parameter<double> ALPHA_X = {
    .name="alpha_x",
    .initial=12.0,
};

constexpr Parameter<double> ALPHA_Y = {
    .name="alpha_y",
    .initial=10.0,
};

constexpr Parameter<double> ALPHA_REF = {
    .name="alpha_ref",
    .initial=14.0,
};

constexpr Parameter<double> BETA_FULL_SCALE = {
    .name="beta_full_scale",
    .initial=20.0,
};

constexpr Parameter<double> BETA_BIAS = {
    .name="beta_bias",
    .initial=0.0,
};

constexpr Parameter<double> BARO_SETTING = {
    .name="baro_setting",
    .initial=29.92,
};

constexpr Parameter<double> BALL_SMOOTHING_FACTOR = {
    .name="ball_smoothing_factor",
    .initial=1.0,
};

constexpr Parameter<double> VSI_SMOOTHING_FACTOR = {
    .name="vsi_smoothing_factor",
    .initial=1.0,
};

constexpr Parameter<int> SCREEN_WIDTH = {
    .name="screen_width",
    .initial=272,
};

constexpr Parameter<int> SCREEN_HEIGHT = {
    .name="screen_height",
    .initial=480,
};

constexpr Parameter<int> SHOW_ALTIMETER = {
    .name="show_altimeter",
    .initial=true,
};

constexpr Parameter<int> SHOW_LINK_STATUS = {
    .name="show_link_status",
    .initial=true,
};

constexpr Parameter<int> SHOW_PROBE_BATTERY_STATUS = {
    .name="show_probe_battery_status",
    .initial=true,
};

Settings::Settings() {
  document_.Parse("{}");
}

void Settings::load_str(std::string str) {
  document_.Parse("{}");
  document_.Parse(str.c_str());
}

void Settings::load(const std::string &path) {
  std::ifstream f;
  f.open(path);
  std::stringstream s;
  s << f.rdbuf();
  f.close();
  load_str(s.str());
}

template<class T>
T Settings::get_value(const Parameter<T> *p) const {
  return p->get(document_);
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

int Settings::screen_width() const {
  return get_value(&SCREEN_WIDTH);
}

int Settings::screen_height() const {
  return get_value(&SCREEN_HEIGHT);
}

bool Settings::show_altimeter() const {
  return get_value(&SHOW_ALTIMETER);
}

bool Settings::show_link_status() const {
  return get_value(&SHOW_LINK_STATUS);
}

bool Settings::show_probe_battery_status() const {
  return get_value(&SHOW_PROBE_BATTERY_STATUS);
}

} // namespace airball

