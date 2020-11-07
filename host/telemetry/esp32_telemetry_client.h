#ifndef AIRBALL_ESP32_TELEMETRY_CLIENT_H
#define AIRBALL_ESP32_TELEMETRY_CLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "telemetry_client.h"
#include "sampler.h"
#include "deadline_tcp_line_reader.h"

namespace airball {

class ESP32TelemetryClient : public TelemetryClient {
public:
  /**
   * Create a client to the Airball probe via an Internet (likely Wi-Fi)
   * connection to its well-known host name and port number.
   */
  explicit ESP32TelemetryClient();
  ~ESP32TelemetryClient();

  std::unique_ptr<sample> get() override;

private:
  deadline_tcp_line_reader reader_;
  sampler telemetry_;
  std::map<std::string, std::ofstream *> files_;
};

}  // namespace airball

#endif //AIRBALL_ESP32_TELEMETRY_CLIENT_H






