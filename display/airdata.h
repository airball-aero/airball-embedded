#ifndef AIRBALL_AIRDATA_H
#define AIRBALL_AIRDATA_H

#include <string>

namespace airball {

class Airdata {
public:
  // Airdata: Indicated air speed
  double ias() const {return ias_;}

  // Airdata: Angle of attack
  double alpha() const {return alpha_;}

  // Airdata: Angle of yaw
  double beta() const {return beta_;}

  // Returns true if the data is valid. If not, display a red X indicating system failure.
  bool valid() const {return valid_;}

  // Commands this model to update its contents based on the given sensor data.
  void update_from_sentence(const std::string& sentence);
private:
  double ias_;
  double alpha_;
  double beta_;
  bool valid_;
};

} // namespace airball

#endif // AIRBALL_AIRDATA_H
