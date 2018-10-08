#include "fake_telemetry_client.h"

#include <ratio>
#include <chrono>
#include <thread>
#include <memory>

#include "units.h"
#include "aerodynamics.h"
#include "../telemetry/sample.h"
#include "../telemetry/airdata_sample.h"
#include "../telemetry/battery_sample.h"

namespace airball {

constexpr static std::chrono::duration<unsigned int, std::milli>
    kSendDelay(20);

constexpr static std::chrono::duration<unsigned int, std::milli>
    kPeriodAirdata(20000);

constexpr static std::chrono::duration<unsigned int, std::milli>
    kPeriodProbeStatus(10000);

constexpr static std::chrono::duration<unsigned int, std::milli>
    kPeriodLinkStatus(10000);

constexpr static unsigned int kInvalidStateNumCycles = 350;

constexpr static std::chrono::duration<unsigned int, std::milli>
    kInvalidStateDelay(3000);

double gage_pressure_at_point(
    double dynamic_pressure,
    double angle_from_stagnation_point) {
  return
      dynamic_pressure *
      (1.0 - 9.0 / 4.0 * pow(cos(angle_from_stagnation_point - M_PI / 2.0), 2));
}

struct Model {
  const double min;
  const double max;
};

constexpr static Model kAirdataBaro{
    .min =  20000,
    .max = 101325,
};

constexpr static Model kAirdataOat{
    .min =   0,
    .max =  20,
};

constexpr static Model kAirdataAlpha{
    .min = degrees_to_radians(0),
    .max = degrees_to_radians(15),
};

constexpr static Model kAirdataBeta{
    .min = degrees_to_radians(-10),
    .max = degrees_to_radians(10),
};

constexpr static Model kAirdataIas{
    .min = knots_to_meters_per_second(100),
    .max = knots_to_meters_per_second(0),
};

constexpr static Model kProbeStatusVoltage{
    .min = 3.2,
    .max = 3.8,
};

constexpr static Model kProbeStatusCurrent{
    .min = 0.025,
    .max = 0.125,
};

constexpr static Model kProbeStatusCapacityMah{
    .min = 2,
    .max = 2,
};

constexpr static Model kProbeStatusCapacityPct{
    .min =   0,
    .max = 100,
};

constexpr static Model kLinkStatusRssi{
    .min = 0,
    .max = 1,
};

constexpr static double kProbeHalfAngle = degrees_to_radians(45);

double interpolate_value(double phase_ratio, const Model &m) {
  double factor = (sin(phase_ratio * 2.0 * M_PI) + 1.0) / 2.0;
  return m.min + factor * (m.max - m.min);
}

uint8_t interpolate_rssi(double phase_ratio) {
  return UINT8_MAX * interpolate_value(phase_ratio, kLinkStatusRssi);
}

double magnitude(double ax, double ay) {
  return sqrt(pow(ax, 2) + pow(ay, 2));
}

double compute_phase_ratio(const std::chrono::steady_clock::duration &period) {
  const long now = std::chrono::steady_clock::now().time_since_epoch().count();
  return (double) (now % period.count()) / (double) period.count();
}

std::unique_ptr<sample> make_airdata(
    std::chrono::time_point<std::chrono::system_clock> time,
    unsigned long seq) {
  const double phase_ratio = compute_phase_ratio(kPeriodAirdata);
  double baro_pressure = interpolate_value(phase_ratio, kAirdataBaro);
  double temperature = interpolate_value(phase_ratio, kAirdataOat);
  double dynamic_pressure = ias_to_q(
      interpolate_value(phase_ratio, kAirdataIas));
  double alpha = interpolate_value(phase_ratio, kAirdataAlpha);
  double beta = interpolate_value(phase_ratio, kAirdataBeta);

  double pressure_center = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha, beta));
  double pressure_top = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha + kProbeHalfAngle, beta));
  double pressure_bottom = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha - kProbeHalfAngle, beta));
  double pressure_left = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha, beta + kProbeHalfAngle));
  double pressure_right = gage_pressure_at_point(
      dynamic_pressure,
      magnitude(alpha, beta - kProbeHalfAngle));

  double delta_p_alpha = pressure_top - pressure_bottom;
  double delta_p_beta = pressure_right - pressure_left;

  return std::unique_ptr<sample>(new airdata_sample(
      time,
      interpolate_rssi(phase_ratio),
      seq,
      baro_pressure,
      temperature,
      dynamic_pressure,
      delta_p_alpha,
      delta_p_beta));
}

std::unique_ptr<sample> make_battery(
    std::chrono::time_point<std::chrono::system_clock> time,
    unsigned long seq) {
  const double phase_ratio = compute_phase_ratio(kPeriodProbeStatus);
  return std::unique_ptr<sample>(new battery_sample(
      time,
      interpolate_rssi(phase_ratio),
      seq,
      interpolate_value(phase_ratio, kProbeStatusVoltage),
      interpolate_value(phase_ratio, kProbeStatusCurrent),
      interpolate_value(phase_ratio, kProbeStatusCapacityMah),
      interpolate_value(phase_ratio, kProbeStatusCapacityPct)));
}

FakeTelemetryClient::FakeTelemetryClient()
    : next_sample_type_(AIRDATA), invalid_state_counter_(0), seq_counter_(0) {}

FakeTelemetryClient::~FakeTelemetryClient() = default;

std::unique_ptr<sample> FakeTelemetryClient::get() {
  const auto now = std::chrono::system_clock::now();
  const auto seq = seq_counter_++;
  switch (next_sample_type_) {
    case SampleType::AIRDATA:
      if (invalid_state_counter_ > kInvalidStateNumCycles) {
        invalid_state_counter_ = 0;
        std::this_thread::sleep_for(kInvalidStateDelay);
      } else {
        invalid_state_counter_++;
        std::this_thread::sleep_for(kSendDelay);
      }
      next_sample_type_ = SampleType::BATTERY;
      return make_airdata(now, seq);
    case BATTERY:
      next_sample_type_ = SampleType::AIRDATA;
      return make_battery(now, seq);
  }
}

}  // namespace airball