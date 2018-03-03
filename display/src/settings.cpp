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
  const double offset;
  const double scale;
  const long int count_min;
  const long int count_max;
  const long int count_default;
};

constexpr const char* ANGULAR_PARAMETER_DISPLAY_UNITS = "°";
constexpr double ANGULAR_PARAMETER_SCALE = 0.05;
constexpr long int ANGULAR_PARAMETER_ABSMAX =
    (long int)(45.0 / ANGULAR_PARAMETER_SCALE);

constexpr Parameter IAS_FULL_SCALE = {
    .name="ias_full_scale",
    .display_name = "IAS FS",
    .display_units = "kt",
    .offset=0,
    .scale=1,
    .count_min=10,
    .count_max=500,
    .count_default=100,
};

constexpr Parameter ALPHA_STALL = {
    .name="alpha_stall",
    .display_name = "α stall",
    ANGULAR_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=ANGULAR_PARAMETER_SCALE,
    .count_min=-ANGULAR_PARAMETER_ABSMAX,
    .count_max=ANGULAR_PARAMETER_ABSMAX,
    .count_default=(long int)(15.0 / ANGULAR_PARAMETER_SCALE),
};

constexpr Parameter ALPHA_MIN = {
    .name="alpha_min",
    .display_name = "α min",
    ANGULAR_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=ANGULAR_PARAMETER_SCALE,
    .count_min=-ANGULAR_PARAMETER_ABSMAX,
    .count_max=ANGULAR_PARAMETER_ABSMAX,
    .count_default=0,
};

constexpr Parameter ALPHA_X = {
    .name="alpha_x",
    .display_name = "α X",
    ANGULAR_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=ANGULAR_PARAMETER_SCALE,
    .count_min=-ANGULAR_PARAMETER_ABSMAX,
    .count_max=ANGULAR_PARAMETER_ABSMAX,
    .count_default=(long int)(12.0 / ANGULAR_PARAMETER_SCALE),
};

constexpr Parameter ALPHA_Y = {
    .name="alpha_y",
    .display_name = "α Y",
    ANGULAR_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=ANGULAR_PARAMETER_SCALE,
    .count_min=-ANGULAR_PARAMETER_ABSMAX,
    .count_max=ANGULAR_PARAMETER_ABSMAX,
    .count_default=(long int)(10.0 / ANGULAR_PARAMETER_SCALE),
};

constexpr Parameter ALPHA_REF = {
    .name="alpha_ref",
    .display_name = "α ref",
    ANGULAR_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=ANGULAR_PARAMETER_SCALE,
    .count_min=-ANGULAR_PARAMETER_ABSMAX,
    .count_max=ANGULAR_PARAMETER_ABSMAX,
    .count_default=(long int)(14.0 / ANGULAR_PARAMETER_SCALE),
};

constexpr Parameter BETA_FULL_SCALE = {
    .name="beta_full_scale",
    .display_name = "β max",
    ANGULAR_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=ANGULAR_PARAMETER_SCALE,
    .count_min=(long int)(5 / ANGULAR_PARAMETER_SCALE),
    .count_max=ANGULAR_PARAMETER_ABSMAX,
    .count_default=(long int)(20.0 / ANGULAR_PARAMETER_SCALE),
};

constexpr Parameter BETA_BIAS = {
    .name="beta_bias",
    .display_name = "β bias",
    ANGULAR_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=ANGULAR_PARAMETER_SCALE,
    .count_min=0,
    .count_max=ANGULAR_PARAMETER_ABSMAX,
    .count_default=0,
};

constexpr const Parameter* ALL_SETTINGS[]{
    &IAS_FULL_SCALE,
    &ALPHA_STALL,
    &ALPHA_MIN,
    &ALPHA_X,
    &ALPHA_Y,
    &ALPHA_REF,
    &BETA_FULL_SCALE,
    &BETA_BIAS,
};

constexpr const int NUM_SETTINGS =
    sizeof(ALL_SETTINGS) / sizeof(Parameter*);

constexpr int NOT_ADJUSTING = -1;

Settings::Settings(std::string path)
    : path_(path), adjusting_param_index_(NOT_ADJUSTING)
{
  for (int i = 0; i < NUM_SETTINGS; i++) {
    values_[ALL_SETTINGS[i]->name] = ALL_SETTINGS[i]->count_default;
  }
  load();
}

void Settings::load_str(std::string str) {
  rapidjson::Document document;
  document.Parse("{}");
  document.Parse(str.c_str());
  for (int i = 0; i < NUM_SETTINGS; i++) {
    auto name = ALL_SETTINGS[i]->name;
    if (document.HasMember(name)) {
      values_[name] = document[name].GetInt();
    }
  }
}

std::string Settings::save_str() {
  rapidjson::Document document;
  document.Parse("{}");
  for (int i = 0; i < NUM_SETTINGS; i++) {
    rapidjson::Value name;
    name.SetString(rapidjson::StringRef(ALL_SETTINGS[i]->name));
    rapidjson::Value value;
    value.SetInt(values_.at(ALL_SETTINGS[i]->name));
    document.AddMember(rapidjson::StringRef(ALL_SETTINGS[i]->name), value, document.GetAllocator());
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);
  return buffer.GetString();
}

void Settings::load() {
  std::ifstream f;
  f.open(path_);
  std::stringstream s;
  s << f.rdbuf();
  f.close();
  load_str(s.str());
}

void Settings::save() {
  std::ofstream f;
  f.open(path_);
  f << save_str();
  f.close();
}

double Settings::get_value(const Parameter* parameter) const {
  return parameter->offset + parameter->scale * values_.at(parameter->name);
}

double Settings::ias_full_scale() const {
  return get_value(&IAS_FULL_SCALE);
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

bool Settings::adjusting() const {
  return adjusting_param_index_ != NOT_ADJUSTING;
}

std::string Settings::adjusting_param_name() const {
  return adjusting()
         ? ALL_SETTINGS[adjusting_param_index_]->display_name
         : "";
}

double Settings::adjusting_param_value() const {
  return adjusting()
         ? get_value(ALL_SETTINGS[adjusting_param_index_])
         : 0;
}

void Settings::set_adjusting(bool adjusting) {
  if (adjusting && adjusting_param_index_ == NOT_ADJUSTING) {
    adjusting_param_index_ = 0;
  }
  if (!adjusting) {
    adjusting_param_index_ = NOT_ADJUSTING;
  }
}

void Settings::adjust_next() {
  if (adjusting()) {
    adjusting_param_index_ = (adjusting_param_index_ + 1) % NUM_SETTINGS;
  }
}

void Settings::adjust_up() {
  if (adjusting()) {
    const Parameter* s = ALL_SETTINGS[adjusting_param_index_];
    values_[s->name] = std::min(s->count_max, values_[s->name] + 1);
  }
  save();
}

void Settings::adjust_down() {
  if (adjusting()) {
    const Parameter* s = ALL_SETTINGS[adjusting_param_index_];
    values_[s->name] = std::max(s->count_min, values_[s->name] - 1);
  }
  save();
}

} // namespace airball

