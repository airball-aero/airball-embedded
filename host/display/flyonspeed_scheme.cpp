#include "flyonspeed_scheme.h"

namespace airball {

constexpr double kPeriodRamp = 0.005; // seconds

constexpr double kToneFrequencyFast = 400; // Hz
constexpr double kToneFrequencySlow = 1600; // Hz

constexpr double kPwmOnPeriodMax = 0.05; // seconds

constexpr double kPwmFrequencyFastLDMax = 1.5; // Hz
constexpr double kPwmFrequencyFastOnspeed = 6.5; // Hz

constexpr double kPwmFrequencySlowOnspeed = 1.5; // Hz
constexpr double kPwmFrequencySlowStall = 20; // Hz

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

void flyonspeed_scheme::update() {
  IAirdata::Ball ball = airdata().smooth_ball();

  // Select FlyONSPEED "fast" or "slow" regime depending on relationship between
  // AoA setpoints and current AoA.
  bool is_fast =
      ball.alpha() > settings().alpha_y() &&
      ball.alpha() < settings().alpha_ref();
  bool is_slow =
      ball.alpha() > settings().alpha_ref();

  if (is_fast || is_slow) {
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

    double tone_frequency = 0.0;
    double pwm_frequency = 0.0;

    if (is_fast) {
      tone_frequency = kToneFrequencyFast;
      double alpha_ratio =
          (ball.alpha() - settings().alpha_y()) /
          (settings().alpha_ref() - settings().alpha_y());
      pwm_frequency =
          kPwmFrequencyFastLDMax +
          alpha_ratio * (kPwmFrequencyFastOnspeed - kPwmFrequencyFastLDMax);
    } else /* is_slow */ {
      tone_frequency = kToneFrequencySlow;
      double alpha_ratio =
          (ball.alpha() - settings().alpha_ref()) /
          (settings().alpha_stall() - settings().alpha_ref());
      pwm_frequency =
          kPwmFrequencySlowOnspeed +
          alpha_ratio * (kPwmFrequencySlowStall - kPwmFrequencySlowOnspeed);
    }

    // Calculate specific PWM periods as integer values
    snd_pcm_uframes_t pwm_period =
        mixer().frequency_to_period(pwm_frequency);
    snd_pcm_uframes_t pwm_on_period =
        std::min(pwm_period, mixer().seconds_to_frames(kPwmOnPeriodMax));
    snd_pcm_uframes_t pwm_ramp_period = ramp_period;
    if (pwm_period - pwm_on_period < (ramp_period * 2)) {
      pwm_on_period = pwm_period;
      pwm_ramp_period = 0;
    }

    // Apply settings
    tone_.set_period(mixer().frequency_to_period(tone_frequency));
    pwm_.set_parameters(pwm_period, pwm_on_period, pwm_ramp_period);

  } else {
    // If not in the "slow" or "fast" regimes, turn off sounds entirely
    balance_.set_gains(0.0, 0.0);
  }
}

} // namespace airball