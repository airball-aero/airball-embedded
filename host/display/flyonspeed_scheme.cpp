#include "flyonspeed_scheme.h"

namespace airball {

// We specify a period for ramping all signals. This applies to the PWM signal
// as well as others. For the PWM signal, we always use a duty cycle of 50%,
// which means that the entire period of the PWM needs to be 4 times the ramp
// period. This constrains the maximum PWM frequency to:
//         (1 / kPeriodRamp * 4)   Hertz
constexpr double kPeriodRamp = 0.005; // seconds ==> max PWM = 50Hz

constexpr double kPeriodDefault = 0.050; // seconds

constexpr double kToneFrequencyFast = 400; // Hz
constexpr double kToneFrequencySlow = 1600; // Hz

constexpr double kPwmFrequencyFastLDMax = 1.5; // Hz
constexpr double kPwmFrequencyFastOnspeed = 6.5; // Hz

constexpr double kPwmFrequencySlowOnspeed = 1.5; // Hz
constexpr double kPwmFrequencySlowStallWarning = 10; // Hz

constexpr double kPwmFrequencyStallWarning = 20; // Hz

constexpr double kAlphaRefToleranceFactor = 0.05;

flyonspeed_scheme::flyonspeed_scheme(
    std::string device_name,
    ISettings *settings,
    IAirdata *airdata)
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

enum speed_regime {
  CRUISE,
  FAST,
  ONSPEED,
  SLOW,
  STALL_WARNING,
};

speed_regime calculate_regime(
    const ISettings& settings,
    const IAirdata::Ball& ball) {
  const double alpha_ref_tolerance =
      kAlphaRefToleranceFactor *
      (settings.alpha_stall() - settings.alpha_ref());

  if (ball.alpha() < settings.alpha_y()) {
    return CRUISE;
  }
  if (ball.alpha() < settings.alpha_ref() - alpha_ref_tolerance) {
    return FAST;
  }
  if (ball.alpha() < settings.alpha_ref() + alpha_ref_tolerance) {
    return ONSPEED;
  }
  if (ball.alpha() < settings.alpha_stall_warning()) {
    return SLOW;
  }
  return STALL_WARNING;
}

double interpolate(
    double y0, double y1,
    double x0, double x1,
    double x) {
  return y0 + (x - x0) / (x1 - x0) * (y1 - y0);
}

void flyonspeed_scheme::update() {
  const IAirdata::Ball ball = airdata().smooth_ball();
  const speed_regime regime = calculate_regime(settings(), ball);

  // Switch off feedback entirely if cruise; else proceed
  switch(regime) {
    case CRUISE:
      balance_.set_gains(0.0, 0.0);
      return;
    default:
      break;
  }

  snd_pcm_uframes_t ramp_period = mixer().seconds_to_frames(kPeriodRamp);

  // Set stereo effect with baseline of (left, right) gains = (0.5, 0.5). Yaw
  // changes that from (1.0, 0.0) in one extreme to (0.0, 1.0) on the other.
  balance_.set_ramp_period(ramp_period);
  double beta_ratio =
      (ball.beta() + settings().beta_bias()) /
      settings().beta_full_scale();
  balance_.set_gains(
      std::min(1.0, (-beta_ratio + 1.0) / 2.0),
      std::min(1.0, (beta_ratio + 1.0) / 2.0));

  // Set tone frequency based on regime.
  double tone_frequency;
  switch (regime) {
    case FAST:
    case ONSPEED:
      tone_frequency = kToneFrequencyFast;
      break;
    case SLOW:
    case STALL_WARNING:
      tone_frequency = kToneFrequencySlow;
      break;
    default:
      return; // error
  }
  tone_.set_period(mixer().frequency_to_period(tone_frequency));

  // Consider the case where the PWM is a constant tone first
  if (regime == ONSPEED) {
    snd_pcm_uframes_t period = mixer().seconds_to_frames(kPeriodDefault);
    pwm_.set_parameters(period, period, 0);
    return;
  }

  // Now consider the cases where we want a PWM signal of some sort.
  double pwm_frequency;
  switch (regime) {
    case FAST:
      pwm_frequency = interpolate(
          kPwmFrequencyFastLDMax, kPwmFrequencyFastOnspeed,
          settings().alpha_y(), settings().alpha_ref(),
          ball.alpha());
      break;
    case SLOW:
      pwm_frequency = interpolate(
          kPwmFrequencySlowOnspeed, kPwmFrequencySlowStallWarning,
          settings().alpha_ref(), settings().alpha_stall_warning(),
          ball.alpha());
      break;
    case STALL_WARNING:
      pwm_frequency = kPwmFrequencyStallWarning;
      break;
    default:
      return; // error
  }

  snd_pcm_uframes_t pwm_period = mixer().frequency_to_period(pwm_frequency);
  snd_pcm_uframes_t pwm_on_period = pwm_period / 2;
  pwm_.set_parameters(pwm_period, pwm_on_period, ramp_period);
}


} // namespace airball