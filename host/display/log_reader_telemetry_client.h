#ifndef AIRBALL_TELEMETRY_LOG_READER_TELEMETRY_CLIENT_H
#define AIRBALL_TELEMETRY_LOG_READER_TELEMETRY_CLIENT_H

#include <fstream>
#include "../telemetry/telemetry_client.h"

namespace airball {

/**
 * A LogReaderTelemetryClient reads logged telemetry from a CSV file on disk.
 */
class LogReaderTelemetryClient : public TelemetryClient {
public:
  LogReaderTelemetryClient(
      const std::string& filename,
      const uint samples_per_second,
      const uint start_sample,
      bool realtime);
  ~LogReaderTelemetryClient();

  std::unique_ptr<sample> get() override;

private:
  const std::string filename;
  const std::chrono::system_clock::duration sample_interval;
  std::chrono::system_clock::time_point last_sample_time;
  const uint start_sample;
  bool realtime_;
  bool skipped_start;
  std::ifstream input;
};

}

#endif  // AIRBALL_TELEMETRY_LOG_READER_TELEMETRY_CLIENT_H