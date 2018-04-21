#include "fake_telemetry_client.h"

#include "gtest/gtest.h"
#include "../airball_probe_telemetry/airdata_sample.h"
#include "../airball_probe_telemetry/battery_sample.h"

namespace airball {

TEST(FakeTelemetryClient, SimpleDataTest) {
  FakeTelemetryClient client;
  ASSERT_TRUE(dynamic_cast<airdata_sample*>(client.get().get()) != nullptr);
  ASSERT_TRUE(dynamic_cast<battery_sample*>(client.get().get()) != nullptr);
}

}