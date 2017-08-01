#include <cmath>
#include <algorithm>
#include <sstream>
#include "interpolation_table.h"

namespace airball {

void InterpolationTable::put(double x, double y) {
  x_y_pairs_.push_back(std::pair<double, double>(x, y));
  sorted_ = false;
}

double InterpolationTable::x(double y) {
  auto compare = [](
      const std::pair<double, double>& a,
      const std::pair<double, double>& b) {
    return a.second < b.second;
  };

  if (!sorted_) {
    std::sort(
        x_y_pairs_.begin(),
        x_y_pairs_.end(),
        compare);
    sorted_ = true;
  }
  auto upper = std::lower_bound(
      x_y_pairs_.begin(),
      x_y_pairs_.end(),
      std::pair<double, double>(0, y),
      compare);
  if (upper == x_y_pairs_.end() || upper == x_y_pairs_.begin()) {
    return std::nan("out of range");
  }
  auto lower = upper - 1;
  return lower->first +
      (upper->first - lower->first) *
      (y - lower->second) / (upper->second - lower->second);
}

std::string InterpolationTable::get_csv() {
  x(0);  // Force table to be sorted
  std::ostringstream os;
  for (auto it = x_y_pairs_.begin(); it < x_y_pairs_.end(); ++it) {
    os << it->first << "," << it->second << std::endl;
  }
  return os.str();
}

} // namespace airball