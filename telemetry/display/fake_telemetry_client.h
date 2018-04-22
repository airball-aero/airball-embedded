#ifndef AIRBALL_TELEMETRY_FAKE_TELEMETRY_CLIENT_H
#define AIRBALL_TELEMETRY_FAKE_TELEMETRY_CLIENT_H

#include <memory>
#include "../airball_probe_telemetry/telemetry_client.h"

namespace airball {

/**
 * A FakeTelemetryClient makes no connections to the outside world; it merely
 * provides a stream of plausible fake data.
 */
class FakeTelemetryClient : public TelemetryClient {
public:
  FakeTelemetryClient();
  ~FakeTelemetryClient();

 std::unique_ptr<sample> get() override;

private:
  enum SampleType {
    AIRDATA,
    BATTERY,
  };

  SampleType next_sample_type_;
  unsigned int invalid_state_counter_;
  unsigned long seq_counter_;
};

}

#endif  // AIRBALL_TELEMETRY_FAKE_TELEMETRY_CLIENT_H