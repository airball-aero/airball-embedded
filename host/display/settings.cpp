/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018, Ihab A.B. Awad
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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

constexpr const char* V_PARAMETER_DISPLAY_UNITS = "kt";
constexpr double V_PARAMETER_SCALE = 1;
constexpr long int V_PARAMETER_MIN = 0;
constexpr long int V_PARAMETER_MAX = 500;
constexpr long int V_PARAMETER_DEFAULT = 100;

constexpr Parameter V_FULL_SCALE = {
    .name="ias_full_scale",
    .display_name = "IAS FS",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=V_PARAMETER_SCALE,
    .count_min=V_PARAMETER_MIN,
    .count_max=V_PARAMETER_MAX,
    .count_default=V_PARAMETER_DEFAULT,
};

constexpr Parameter V_R = {
    .name="v_r",
    .display_name = "Vr",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=V_PARAMETER_SCALE,
    .count_min=V_PARAMETER_MIN,
    .count_max=V_PARAMETER_MAX,
    .count_default=V_PARAMETER_DEFAULT,
};

constexpr Parameter V_FE = {
    .name="v_fe",
    .display_name = "Vfe",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=V_PARAMETER_SCALE,
    .count_min=V_PARAMETER_MIN,
    .count_max=V_PARAMETER_MAX,
    .count_default=V_PARAMETER_DEFAULT,
};

constexpr Parameter V_NO = {
    .name="v_no",
    .display_name = "Vno",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=V_PARAMETER_SCALE,
    .count_min=V_PARAMETER_MIN,
    .count_max=V_PARAMETER_MAX,
    .count_default=V_PARAMETER_DEFAULT,
};

constexpr Parameter V_NE = {
    .name="v_ne",
    .display_name = "Vne",
    .display_units = V_PARAMETER_DISPLAY_UNITS,
    .offset=0,
    .scale=V_PARAMETER_SCALE,
    .count_min=V_PARAMETER_MIN,
    .count_max=V_PARAMETER_MAX,
    .count_default=V_PARAMETER_DEFAULT,
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
    .count_min=-ANGULAR_PARAMETER_ABSMAX,
    .count_max=ANGULAR_PARAMETER_ABSMAX,
    .count_default=0,
};

constexpr Parameter BARO_SETTING = {
    .name="baro_setting",
    .display_name = "baro",
    "in Hg",
    .offset=0,
    .scale=0.01,
    .count_min=2792,
    .count_max=3192,
    .count_default=2992,
};

constexpr Parameter BALL_SMOOTHING_FACTOR = {
    .name="ball_smoothing_factor",
    .display_name = "SF ball",
    "",
    .offset=0,
    .scale=0.005,
    .count_min=0,
    .count_max=200,
    .count_default=200,
};

constexpr Parameter VSI_SMOOTHING_FACTOR = {
    .name="vsi_smoothing_factor",
    .display_name = "SF vsi",
    "",
    .offset=0,
    .scale=0.005,
    .count_min=0,
    .count_max=200,
    .count_default=200,
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

constexpr const Parameter* SEQUENCED_SETTINGS[]{
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
    &BALL_SMOOTHING_FACTOR,
    &VSI_SMOOTHING_FACTOR,
};

constexpr const int NUM_ALL_SETTINGS =
    sizeof(ALL_SETTINGS) / sizeof(Parameter*);

constexpr const int NUM_SEQUENCED_SETTINGS =
    sizeof(SEQUENCED_SETTINGS) / sizeof(Parameter*);

constexpr int NOT_ADJUSTING = -1;

Settings::Settings(std::string path)
    : path_(path), adjusting_param_index_(NOT_ADJUSTING)
{
  for (int i = 0; i < NUM_ALL_SETTINGS; i++) {
    values_[ALL_SETTINGS[i]->name] = ALL_SETTINGS[i]->count_default;
  }
  load();
}

void Settings::load_str(std::string str) {
  rapidjson::Document document;
  document.Parse("{}");
  document.Parse(str.c_str());
  for (int i = 0; i < NUM_ALL_SETTINGS; i++) {
    auto name = ALL_SETTINGS[i]->name;
    if (document.HasMember(name)) {
      values_[name] = document[name].GetInt();
    }
  }
}

std::string Settings::save_str() {
  rapidjson::Document document;
  document.Parse("{}");
  for (int i = 0; i < NUM_ALL_SETTINGS; i++) {
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

double Settings::get_value(const Parameter* p) const {
  return p->offset + p->scale * values_.at(p->name);
}

void Settings::adjust_param_up(const Parameter* p) {
  values_[p->name] = std::min(p->count_max, values_[p->name] + 1);
  save();
}

void Settings::adjust_param_down(const Parameter* p) {
  values_[p->name] = std::max(p->count_min, values_[p->name] - 1);
  save();
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

void Settings::adjust_baro_setting_up() {
  adjust_param_up(&BARO_SETTING);
}

void Settings::adjust_baro_setting_down() {
  adjust_param_down(&BARO_SETTING);
}

bool Settings::adjusting() const {
  return adjusting_param_index_ != NOT_ADJUSTING;
}

std::string Settings::adjusting_param_name() const {
  return adjusting()
         ? SEQUENCED_SETTINGS[adjusting_param_index_]->display_name
         : "";
}

double Settings::adjusting_param_value() const {
  return adjusting()
         ? get_value(SEQUENCED_SETTINGS[adjusting_param_index_])
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
    adjusting_param_index_ = (adjusting_param_index_ + 1) % NUM_SEQUENCED_SETTINGS;
  }
}

void Settings::adjust_up() {
  if (adjusting()) {
    adjust_param_up(SEQUENCED_SETTINGS[adjusting_param_index_]);
  }
}

void Settings::adjust_down() {
  if (adjusting()) {
    adjust_param_down(SEQUENCED_SETTINGS[adjusting_param_index_]);
  }
}

} // namespace airball

