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

 Datum get() override;

private:
  TelemetryClient::DatumType  next_datum_;
};

}

#endif  // AIRBALL_TELEMETRY_FAKE_TELEMETRY_CLIENT_H