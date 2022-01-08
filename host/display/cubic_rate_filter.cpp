#include <complex>
#include "cubic_rate_filter.h"

#include "polyfit.h"

namespace airball {

cubic_rate_filter::cubic_rate_filter(int size) {
  set_size(size);
}

int cubic_rate_filter::size() const {
  return values_y_.size();
}

void cubic_rate_filter::set_size(int size) {
  values_y_.resize(size, 0);
}

void cubic_rate_filter::put(double y) {
  values_y_.pop_back();
  values_y_.insert(values_y_.begin(), y);
  compute_rate();
}

double cubic_rate_filter::get_rate() {
  return rate_;
}

void cubic_rate_filter::compute_rate() {
  std::vector<double> coeff;
  std::vector<double> values_x;
  for (int i = 0; i < values_y_.size(); i++) {
    values_x.insert(values_x.begin(), (double) i);
  }
  polyfit(values_x, values_y_, coeff, 3);
  const double x = values_x[0];
  rate_ = 0;
  for (int i = 1; i < coeff.size(); i++) {
    rate_ += coeff[i] * ((double) i) * std::pow(x, i - 1);
  }
}

}