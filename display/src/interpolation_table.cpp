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