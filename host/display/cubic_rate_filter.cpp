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
  values_x_.resize(size, 0);
  values_y_.resize(size, 0);
  for (int i = 0; i < size; i++) {
    values_x_[i] = double(-i);
  }
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
  polyfit(values_x_, values_y_, coeff, 3);
  rate_ = coeff[1];
}

}