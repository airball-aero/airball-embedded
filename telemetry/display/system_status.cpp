#include "system_status.h"

namespace airball {

constexpr static std::chrono::duration<unsigned int, std::milli>
    kAirdataExpiryPeriod(250);

SystemStatus::SystemStatus()
    : link_quality_(0),
      battery_health_(0),
      last_airdata_time_(std::chrono::system_clock::from_time_t(0))
{}

bool SystemStatus::flight_data_up() const {
  std::chrono::system_clock::duration time_since_airdata =
      std::chrono::system_clock::now().time_since_epoch() -
      last_airdata_time_.time_since_epoch();
  return time_since_airdata < kAirdataExpiryPeriod;
}

double SystemStatus::link_quality() const {
  return link_quality_;
}

double SystemStatus::battery_health() const {
  return battery_health_;
}

void SystemStatus::update(TelemetryClient::Datum d) {
  switch (d.type) {
    case TelemetryClient::AIRDATA:
      update(d.airdata);
      break;
    case TelemetryClient::LINK_STATUS:
      update(d.link_status);
      break;
    case TelemetryClient::PROBE_STATUS:
      update(d.probe_status);
      break;
  }
}

void SystemStatus::update(TelemetryClient::Airdata d) {
  last_airdata_time_ = std::chrono::system_clock::now();
}

void SystemStatus::update(TelemetryClient::LinkStatus d) {
  link_quality_ = d.rssi_ratio;
}

void SystemStatus::update(TelemetryClient::ProbeStatus d) {
  battery_health_ = d.capacity_ratio;
}

}  // namespace airball