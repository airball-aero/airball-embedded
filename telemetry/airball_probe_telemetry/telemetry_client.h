#ifndef AIRBALL_TELEMETRY_CLIENT_H
#define AIRBALL_TELEMETRY_CLIENT_H

#include <memory>
#include "sample.h"

namespace airball {

/**
 * A client that receives telemetry from an Airball probe.
 */
class TelemetryClient {
public:
  TelemetryClient() {}
  virtual ~TelemetryClient() {}

  /**
   * Blocks until a sample is available, then returns it.
   */
  virtual std::unique_ptr<sample> get() = 0;
};

}  // namespace airball

#endif // AIRBALL_TELEMETRY_CLIENT_H
