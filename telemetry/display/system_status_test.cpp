#include <gtest/gtest.h>
#include <thread>
#include "system_status.h"

constexpr static std::chrono::duration<unsigned int, std::milli>
    kDeadDataDelay(500);

TEST(SystemStatus, AirdataExpiry) {
  airball::SystemStatus s;
  airball::TelemetryClient::Datum d;
  d.type = airball::TelemetryClient::AIRDATA;
  d.airdata = {
      .baro = 0,
      .oat = 0,
      .dp0 = 0,
      .dpA = 0,
      .dpB = 0,
  };
  s.update(d);
  ASSERT_TRUE(s.flight_data_up());
  // TODO(ihab): Inject a clock rather than using real time
  std::this_thread::sleep_for(kDeadDataDelay);
  ASSERT_FALSE(s.flight_data_up());
  s.update(d);
  ASSERT_TRUE(s.flight_data_up());
}

TEST(SystemStatus, Battery) {
  airball::SystemStatus s;
  ASSERT_EQ(s.battery_health(), 0.0);
  airball::TelemetryClient::Datum d;
  d.type = airball::TelemetryClient::PROBE_STATUS;
  d.probe_status = {
      .voltage = 0.6,
      .current = 0.5,
      .capacity = 0.4,
      .capacity_ratio = 0.3,
  };
  s.update(d);
  ASSERT_EQ(s.battery_health(), 0.3);
}

TEST(SystemStatus, Link) {
  airball::SystemStatus s;
  ASSERT_EQ(s.link_quality(), 0.0);
  airball::TelemetryClient::Datum d;
  d.type = airball::TelemetryClient::LINK_STATUS;
  d.link_status = {
      .rssi_ratio = 0.3,
  };
  s.update(d);
  ASSERT_EQ(s.link_quality(), 0.3);
}
