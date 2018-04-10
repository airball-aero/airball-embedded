#include "fake_telemetry_client.h"

#include <ratio>
#include <chrono>
#include <thread>

#include "units.h"
#include "aerodynamics.h"

namespace airball {

constexpr static std::chrono::duration<unsigned int, std::milli>
    kSendDelay(20);

constexpr static std::chrono::duration<unsigned int, std::milli>
    kPeriodAirdata(5000);

constexpr static std::chrono::duration<unsigned int, std::milli>
    kPeriodProbeStatus(10000);

constexpr static std::chrono::duration<unsigned int, std::milli>
    kPeriodLinkStatus(10000);

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

constexpr static Model kProbeStatusCapacity{
    .min = 2,
    .max = 2,
};

constexpr static Model kProbeStatusCapacityRatio{
    .min = 0,
    .max = 1,
};

constexpr static Model kLinkStatusRssiRatio{
    .min = 0,
    .max = 1,
};

constexpr static double kProbeHalfAngle = degrees_to_radians(45);

double interpolate_value(double phase_ratio, const Model &m) {
  double factor = (sin(phase_ratio * 2.0 * M_PI) + 1.0) / 2.0;
  return m.min + factor * (m.max - m.min);
}

double magnitude(double ax, double ay) {
  return sqrt(pow(ax, 2) + pow(ay, 2));
}

double compute_phase_ratio(const std::chrono::steady_clock::duration &period) {
  const long now = std::chrono::steady_clock::now().time_since_epoch().count();
  return (double) (now % period.count()) / (double) period.count();
}

TelemetryClient::Airdata make_airdata() {
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

  return TelemetryClient::Airdata{
      .baro = baro_pressure,
      .oat = temperature,
      .dp0 = pressure_center,
      .dpA = delta_p_alpha,
      .dpB = delta_p_beta,
  };
}

TelemetryClient::ProbeStatus make_probe_status() {
  const double phase_ratio = compute_phase_ratio(kPeriodProbeStatus);
  return TelemetryClient::ProbeStatus{
      .voltage = interpolate_value(phase_ratio, kProbeStatusVoltage),
      .current = interpolate_value(phase_ratio, kProbeStatusCurrent),
      .capacity = interpolate_value(phase_ratio, kProbeStatusCapacity),
      .capacity_ratio = interpolate_value(phase_ratio, kProbeStatusCapacityRatio),
  };
}

TelemetryClient::LinkStatus make_link_status() {
  const double phase_ratio = compute_phase_ratio(kPeriodLinkStatus);
  return TelemetryClient::LinkStatus{
      .rssi_ratio = interpolate_value(phase_ratio, kLinkStatusRssiRatio)
  };
}

FakeTelemetryClient::FakeTelemetryClient()
    : next_datum_(AIRDATA) {}

FakeTelemetryClient::~FakeTelemetryClient() = default;

TelemetryClient::Datum FakeTelemetryClient::get() {
  Datum d;
  switch (next_datum_) {
    case AIRDATA:
      std::this_thread::sleep_for(kSendDelay);
      next_datum_ = PROBE_STATUS;
      d.type = AIRDATA;
      d.airdata = make_airdata();
      return d;
    case PROBE_STATUS:
      next_datum_ = LINK_STATUS;
      d.type = PROBE_STATUS;
      d.probe_status = make_probe_status();
      return d;
    case LINK_STATUS:
      next_datum_ = AIRDATA;
      d.type = LINK_STATUS;
      d.link_status = make_link_status();
      return d;
  }
}

}  // namespace airball