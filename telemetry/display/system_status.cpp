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

void SystemStatus::update(const sample* d) {
  link_quality_ = ((double)d->get_rssi()) / ((double)UINT8_MAX);
  update(dynamic_cast<const airdata_sample*>(d));
  update(dynamic_cast<const battery_sample*>(d));
}

void SystemStatus::update(const airdata_sample* d) {
  if (d == nullptr) return;
  last_airdata_time_ = std::chrono::system_clock::now();
}

void SystemStatus::update(const battery_sample* d) {
  if (d == nullptr) return;
  battery_health_ = d->get_capacty_pct() / 100.0;
}

}  // namespace airball