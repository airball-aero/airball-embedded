#include "system_status.h"

namespace airball {

constexpr static uint8_t kRssiMin = 0x30;
constexpr static uint8_t kRssiMax = 0x4f;

constexpr static std::chrono::duration<unsigned int, std::milli>
    kAirdataExpiryPeriod(250);

SystemStatus::SystemStatus()
    : link_quality_(0),
      battery_health_(0),
      battery_charging_(false),
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

bool SystemStatus::battery_charging() const {
  return battery_charging_;
}

double SystemStatus::battery_voltage() const {
  return battery_sample_.get_voltage();
}

double SystemStatus::battery_current() const {
  return battery_sample_.get_current();
}

void SystemStatus::update(const sample* d) {
  double min = (double)kRssiMin;
  double max = (double)kRssiMax;
  double value = (double)d->get_rssi();
  // Lower RSSI values indicate better signal, hence the below formula:
  //     value == min  ==>  quality = 1
  //     value == max  ==>  quality = 0
  link_quality_ = (value - max) / (min - max);
  link_quality_ = std::min(1.0, link_quality_);
  link_quality_ = std::max(0.0, link_quality_);
  update(dynamic_cast<const airdata_sample*>(d));
  update(dynamic_cast<const battery_sample*>(d));
}

void SystemStatus::update(const airdata_sample* d) {
  if (d == nullptr) return;
  last_airdata_time_ = std::chrono::system_clock::now();
}

void SystemStatus::update(const battery_sample* d) {
  if (d == nullptr) return;
  battery_sample_ = *d;
  battery_health_ = std::min(1.0, (d->get_voltage() - 3.3) / 0.9);
  battery_charging_ = d->get_current() > 0.0;
}

}  // namespace airball
