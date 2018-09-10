#ifndef AIRBALL_TELEMETRY_SYSTEM_STATUS_H
#define AIRBALL_TELEMETRY_SYSTEM_STATUS_H

#include <chrono>

#include "../airball_probe_telemetry/telemetry_client.h"
#include "../airball_probe_telemetry/airdata_sample.h"
#include "../airball_probe_telemetry/battery_sample.h"

namespace airball {

/**
 * A SystemStatus keeps track of all the information about the system that
 * is not covered by Airdata or Settings.
 */
class SystemStatus {
public:
  SystemStatus();

  /**
   * Adds a datum to the system status. All data from the probe is expected to
   * be sent here.
   */
  void update(const sample* d);

  /**
   * Returns the overall health status of the system. If this returns false, the
   * user interface must show an INOP notification overriding all flight data.
   */
  bool flight_data_up() const;

  /**
   * Provides a single metric of wireless link quality, ranging from 0.0 to 1.0,
   * suitable for displaying to the user.
   */
  double link_quality() const;

  /**
   * Provides a single "fuel gage" of probe battery life, ranging from
   * 0.0 to 1.0, suitable for displaying to the user.
   */
  double battery_health() const;

  /**
   * Indicates whether the battery is currently charging.
   */
  bool battery_charging() const;

  /**
   * The voltage measured in the most recent battery sampling.
   */
  double battery_voltage() const;

  /**
   * The current measured in the most recent battery sampling in mA.
   * Negative values indicate discharging; positive indicate charging.
   */
  double battery_current() const;

private:
  void update(const airdata_sample* d);
  void update(const battery_sample* d);

  double link_quality_;
  double battery_health_;
  bool battery_charging_;
  battery_sample battery_sample_;

  std::chrono::system_clock::time_point last_airdata_time_;
};

}  // namespace airball

#endif  // AIRBALL_TELEMETRY_SYSTEM_STATUS_H
