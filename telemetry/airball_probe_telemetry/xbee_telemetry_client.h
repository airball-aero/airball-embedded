#ifndef AIRBALL_XBEE_TELEMETRY_CLIENT_H
#define AIRBALL_XBEE_TELEMETRY_CLIENT_H

#include "telemetry_client.h"
#include "sampler.h"
#include "xbee.h"

namespace airball {

class XbeeTelemetryClient : public TelemetryClient {
public:
  /**
   * Create a client to the Airball probe via a local serial connection to an
   * XBee radio. Setup work will be done in the constructor, so the various
   * devices need to be ready at the time of instantiation.
   *
   * @param serial_device_filename the filename of the serial device to which
   *     the XBee radio is connected, e.g., "/dev/ttyUSB0".
   */
  XbeeTelemetryClient(const std::string& serial_device_filename);
  ~XbeeTelemetryClient();

  std::unique_ptr<sample> get() override;

private:
  const std::string serial_device_filename_;
  xbee radio_;
  sampler telemetry_;
  std::map<std::string, std::ofstream *> files_;
  std::map<std::string, int> stats_;
};

}  // namespace airball

#endif //AIRBALL_XBEE_TELEMETRY_CLIENT_H
