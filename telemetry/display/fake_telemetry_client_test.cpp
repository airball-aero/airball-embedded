#include "fake_telemetry_client.h"

#include "gtest/gtest.h"

namespace airball {

TEST(FakeTelemetryClient, SimpleDataTest) {
  FakeTelemetryClient client;
  auto d0 = client.get();
  ASSERT_EQ(TelemetryClient::AIRDATA, d0.type);
  auto d1 = client.get();
  ASSERT_EQ(TelemetryClient::PROBE_STATUS, d1.type);
  auto d2 = client.get();
  ASSERT_EQ(TelemetryClient::LINK_STATUS, d2.type);
}

}