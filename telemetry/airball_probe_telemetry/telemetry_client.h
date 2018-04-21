#ifndef AIRBALL_TELEMETRY_CLIENT_H
#define AIRBALL_TELEMETRY_CLIENT_H

#include <memory>
#include "sample.h"

namespace airball {

/**
 * A client to a standard Airball telemetry server. A telemetry server
 * runs on a Linux machine and receives telemetry from some source.
 */
class TelemetryClient {
public:
  /**
   * Blocks until a sample is available, then returns it.
   */
  virtual std::unique_ptr<sample> get() = 0;
};

}  // namespace airball

#endif //AIRBALL_TELEMETRY_CLIENT_H
