#include "airdata.h"

#include <string>
#include <iostream>

#include "aerodynamics.h"
#include "units.h"

namespace airball {

void Airdata::update_from_sentence(const std::string& sentence) {
  double baro;
  double delta_p_0;
  double delta_p_alpha;
  double delta_p_beta;
  int cnt = sscanf(
      sentence.c_str(),
      "%lf,%lf,%lf,%lf",
      &baro, &delta_p_0, &delta_p_alpha, &delta_p_beta);
  if (cnt != 4) {
    std::cout << "Could not parse sentence \"" << sentence << "\"" << std::endl;
    valid_ = false;
    return;
  }
  ias_ = meters_per_second_to_knots(q_to_ias(delta_p_0));
  // std::cout << "  ratio alpha =" << delta_p_alpha / delta_p_0 << std::endl;
  // std::cout << "  ratio beta  =" << delta_p_beta / delta_p_0 << std::endl;
  alpha_ = - delta_p_alpha / delta_p_0 * 20.0 / 2.0;
  beta_ = delta_p_beta / delta_p_0 * 20.0 / 2.0;
  valid_ = true;
}

} // namespace airball

