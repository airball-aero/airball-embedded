#include <iostream>
#include "windrush_scheme.h"

namespace airball {

constexpr double kPeriodRamp = 0.005; // seconds

constexpr double kFrequencyAlphaMin = 220; // hertz
constexpr double kOctavesMinToMax = 2; // octaves

constexpr double kPwmDutyCycleAlphaMin = 0.5;
constexpr double kPwmDutyCycleAlphaMax = 1.0;

constexpr double kPwmFrequencyAlphaMin = 1; // hertz
constexpr double kPwmFrequencyAlphaMax = 5; //hertz

windrush_scheme::windrush_scheme(
    std::string device_name,
    ISettings* settings,
    IAirdata* airdata)
    : sound_scheme(device_name, settings, airdata),
      tone_(1),
      pwm_(1, 1, 1),
      balance_(1, 1, 1) {
  mixer().set_layers({
      &tone_,
      &pwm_,
      &balance_,
  });
}

void windrush_scheme::update() {
  IAirdata::Ball ball = airdata().smooth_ball();
  double alpha_ratio =
      (ball.alpha() - settings().alpha_min()) /
      (settings().alpha_stall() - settings().alpha_min());
  tone_.set_period(
      mixer().octaves_to_period(
          kFrequencyAlphaMin,
          alpha_ratio * kOctavesMinToMax));
  snd_pcm_uframes_t pwm_period = mixer().frequency_to_period(
      kPwmFrequencyAlphaMin +
      alpha_ratio * (kPwmFrequencyAlphaMax - kPwmFrequencyAlphaMin));
  snd_pcm_uframes_t pwm_on_period = (snd_pcm_uframes_t)
      ((double) pwm_period *
      (kPwmDutyCycleAlphaMin +
      alpha_ratio * (kPwmDutyCycleAlphaMax - kPwmDutyCycleAlphaMin)));
  snd_pcm_uframes_t pwm_fade_period = mixer().seconds_to_frames(kPeriodRamp);
  if (pwm_fade_period > (pwm_period - pwm_on_period) / 2) {
    pwm_fade_period = 0;
    pwm_on_period = pwm_period;
  }
  pwm_.set_parameters(pwm_period, pwm_on_period, pwm_fade_period);
  double beta_ratio =
      (ball.beta() + settings().beta_bias()) /
      settings().beta_full_scale();
  balance_.set_ramp_period(
      mixer().seconds_to_frames(kPeriodRamp));
  if (ball.alpha() < settings().alpha_ref()) {
    balance_.set_gains(
        std::max(0.0, -beta_ratio * 0.5),
        std::max(0.0, beta_ratio * 0.5));
  } else if (ball.alpha() < settings().alpha_stall()) {
    double alpha_high_range_ratio =
        (ball.alpha() - settings().alpha_ref()) /
        (settings().alpha_stall() - settings().alpha_ref());
    balance_.set_gains(
        std::min(1.0, -beta_ratio * 0.5 + alpha_high_range_ratio * 0.5),
        std::min(1.0, beta_ratio * 0.5 + alpha_high_range_ratio * 0.5));
  } else {
    balance_.set_gains(1.0, 1.0);
  }
}

} // namespace airball