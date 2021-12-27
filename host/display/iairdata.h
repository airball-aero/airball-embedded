#ifndef AIRBALL_DISPLAY_IAIRDATA_H
#define AIRBALL_DISPLAY_IAIRDATA_H

namespace airball {

class IAirdata {
public:
  class Ball {
  public:
    Ball(double alpha, double beta, double ias, double tas)
    : alpha_(alpha), beta_(beta), ias_(ias), tas_(tas) {}
    Ball() : Ball(0, 0, 0, 0) {}
    ~Ball() = default;

    double alpha() const { return alpha_; }
    double beta() const { return beta_; }
    double ias() const { return ias_; }
    double tas() const { return tas_; }

  private:
    double alpha_;
    double beta_;
    double ias_;
    double tas_;
  };

  // Current altitude
  virtual double altitude() const = 0;

  // Current climb rate
  virtual double climb_rate() const = 0;

  // Returns true if the data is valid. If not, display a red X indicating system failure.
  virtual bool valid() const = 0;

  virtual const Ball& smooth_ball() const = 0;

  virtual const std::vector<Ball>& raw_balls() const = 0;

};

}

#endif // AIRBALL_DISPLAY_IAIRDATA_H