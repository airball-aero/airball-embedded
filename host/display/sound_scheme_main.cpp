#include <iostream>
#include <thread>
#include <chrono>
#include <boost/format.hpp>
#include <termios.h>

#include "flyonspeed_scheme.h"
#include "windrush_scheme.h"
#include "isettings.h"
#include "iairdata.h"

constexpr double kAlphaBetaStep = 0.01;
constexpr int kDisplayHalfSize = 20;

class TestSettings : public airball::ISettings {
public:
  double v_full_scale() const override { return v_full_scale_; }
  double v_r() const override { return v_r_; }
  double v_fe() const override { return v_fe_; }
  double v_no() const override { return v_no_; }
  double v_ne() const override { return v_ne_; }
  double alpha_stall() const override { return alpha_stall_; }
  double alpha_min() const override { return alpha_min_; }
  double alpha_x() const override { return alpha_x_; }
  double alpha_y() const override { return alpha_y_; }
  double alpha_ref() const override { return alpha_ref_; }
  double beta_full_scale() const override { return beta_full_scale_; }

  double beta_bias() const override { return 0; }
  double baro_setting() const override { return 0; }
  double ball_smoothing_factor() const override { return 0; }
  double vsi_smoothing_factor() const override { return 0; }
  int screen_width() const override { return 0; }
  int screen_height() const override { return 0; }
  bool show_altimeter() const override { return false; }
  bool show_link_status() const override { return false; }
  bool show_probe_battery_status() const override { return false; }

  double v_full_scale_ = 0;
  double v_r_ = 0;
  double v_fe_ = 0;
  double v_no_ = 0;
  double v_ne_ = 0;
  double alpha_stall_ = 0;
  double alpha_min_ = 0;
  double alpha_x_ = 0;
  double alpha_y_ = 0;
  double alpha_ref_ = 0;
  double beta_full_scale_ = 0;
};

class TestAirdata : public airball::IAirdata {
public:
  TestAirdata() {
    balls_.emplace_back(Ball(0.0, 0.0, 0.0, 0.0));
  }

  double altitude() const override { return 0; }
  double climb_rate() const override {  return 0; }
  bool valid() const override { return true; }

  const Ball &smooth_ball() const override { return balls_[0]; }
  const std::vector<Ball> &raw_balls() const override { return balls_; }

  void set_ball(Ball b) { balls_[0]= b; }

private:
  std::vector<Ball> balls_;
};

void print_alpha_beta(double alpha, double beta) {
  std::cout << boost::format("%05.02d") % alpha;
  int k = beta * kDisplayHalfSize;
  std::cout << "    " << "|";
  for (int i = -kDisplayHalfSize; i <= kDisplayHalfSize; i++) {
    if (i == k) {
      std::cout << "*";
    } else if (i == 0) {
      std::cout << ".";
    } else {
      std::cout << " ";
    }
  }
  std::cout << "|" << std::endl;
}

int main(int argc, char**argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <device-name> <scheme-name>" << std::endl;
    return -1;
  }

  TestSettings settings;
  settings.alpha_min_ = 0.0;
  settings.alpha_y_ = 0.5;
  settings.alpha_ref_ = 0.75;
  settings.alpha_stall_ = 1.0;
  settings.beta_full_scale_ = 1.0;

  TestAirdata airdata;

  std::string device_name(argv[1]);
  std::string scheme_name(argv[2]);
  std::unique_ptr<airball::sound_scheme> scheme;

  if (scheme_name == "flyonspeed") {
    scheme.reset(new airball::flyonspeed_scheme(
        device_name,
        &settings,
        &airdata));
  } else if (scheme_name == "windrush") {
    scheme.reset(new airball::windrush_scheme(
        device_name,
        &settings,
        &airdata));
  } else {
    std::cerr << "Unknown scheme: " << scheme_name << std::endl;
    return -1;
  }

  if (!scheme->start()) {
    std::cout << "Error" << std::endl;
    return -1;
  }

  auto set_alpha_beta = [&](double alpha, double beta) {
    print_alpha_beta(alpha, beta);
    airdata.set_ball(airball::IAirdata::Ball(alpha, beta, 0.0, 0.0));
    scheme->update();
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(10));
  };

  double alpha = 0.0;
  double beta = 0.0;

  set_alpha_beta(alpha, beta);

  struct termios old_settings;
  tcgetattr(STDIN_FILENO, &old_settings);
  struct termios new_settings = old_settings;
  new_settings.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

  for (bool running = true; running; ) {
    char d = 0;
    if (!read(STDIN_FILENO, &d, 1)) {
      break;
    }
    bool set = true;
    switch (d) {
      case 'u':
        alpha += kAlphaBetaStep;
        break;
      case 'd':
        alpha -= kAlphaBetaStep;
        break;
      case 'r':
        beta += kAlphaBetaStep;
        break;
      case 'l':
        beta -= kAlphaBetaStep;
        break;
      case 'c':
        beta = 0;
        break;
      case 'x':
        running = false;
        set = false;
        break;
      default:
        set = false;
        break;
    }
    if (set) {
      set_alpha_beta(alpha, beta);
    }
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

  return 0;
}
