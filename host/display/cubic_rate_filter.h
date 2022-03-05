#ifndef AIRBALL_CUBIC_RATE_FILTER_H_
#define AIRBALL_CUBIC_RATE_FILTER_H_

#include <vector>

namespace airball {

class cubic_rate_filter {
public:
  cubic_rate_filter(int size);

  int size() const;
  void set_size(int size);

  void put(double y);

  double get_rate();

private:
  void compute_rate();

  std::vector<double> values_x_;
  std::vector<double> values_y_;
  double rate_;
};

} // namespace airball

#endif // AIRBALL_CUBIC_RATE_FILTER_H_
