#include "xbee_telemetry_client.h"

#include <fstream>

#include "airdata_sample.h"
#include "battery_sample.h"
#include "format.h"
#include "xbee_api_payload.h"
#include "xbee_known_types.h"

namespace airball {

XbeeTelemetryClient::XbeeTelemetryClient(
    const std::string& serial_device_filename)
    : serial_device_filename_(serial_device_filename),
      radio_(serial_device_filename_, 19200) {
  radio_.enter_command_mode();

  radio_.send_command("ATNIAIRBALL_BASE");
  radio_.send_command("ATID=5555");
  radio_.send_command("ATMY=8888");
  radio_.send_command("ATAP=0");

  radio_.exit_command_mode();

  telemetry_.add_sample_type(airdata_sample::PREFIX, airdata_sample::create);
  telemetry_.add_sample_type(battery_sample::PREFIX, battery_sample::create);

  // This is really dumb.
  std::vector<std::string> file_types;
  file_types.emplace_back("airdata");
  file_types.emplace_back("battery");

  stats_["unusable"] = 0;
  stats_["exception"] = 0;

  auto log_time = std::chrono::system_clock::now();

  for (const std::string &type : file_types) {
    std::ofstream *file = new std::ofstream;
    std::string filename = airball::format_log_filename(log_time, type);
    file->open(filename, std::ios::out);
    files_[type] = file;
    stats_[type] = 0;
  }
}

XbeeTelemetryClient::~XbeeTelemetryClient() {
  for (auto f = files_.begin(); f != files_.end(); ++f) {
    (*f).second->close();
  }
};

std::unique_ptr<sample> XbeeTelemetryClient::get() {
  while (true) {
    try {
      std::string data = radio_.get_line();
      uint8_t rssi = 0; // TODO: Support RSSI
      auto current_time = std::chrono::system_clock::now();
      if (sample *s = telemetry_.parse(current_time, rssi, data)) {
        stats_[s->type()]++;
        *files_[s->type()] << s->format().c_str() << std::endl;
        files_[s->type()]->flush();
        return std::unique_ptr<sample>(s);
      } else {
        std::cout << "unusable: " << data << std::endl;
        stats_["unusable"]++;
      }
    } catch (const std::exception &e) {
      stats_["exception"]++;
    }
  }
}

} // namespace airball