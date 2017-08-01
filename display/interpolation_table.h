#ifndef AIRBALL_INTERPOLATION_TABLE_H
#define AIRBALL_INTERPOLATION_TABLE_H

#include <vector>

namespace airball {

/**
 * An InterpolationTable maintains a table of X and Y values. Given a Y value,
 *
 */
class InterpolationTable {

public:
  InterpolationTable() {}
  ~InterpolationTable() {}

  void put(double x, double y);

  double x(double y);

  std::string get_csv();

private:
  std::vector<std::pair<double, double>> x_y_pairs_;
  bool sorted_ = false;
};

} // namespace airball

#endif // AIRBALL_INTERPOLATION_TABLE_H
