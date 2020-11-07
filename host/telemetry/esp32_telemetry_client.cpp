#include "esp32_telemetry_client.h"

#include <fstream>
#include <boost/algorithm/string/trim.hpp>

#include "airdata_reduced_sample.h"
#include "battery_sample.h"
#include "format.h"

namespace airball {

ESP32TelemetryClient::ESP32TelemetryClient()
    : reader_("192.168.4.1", "80", boost::posix_time::seconds(1)) {
  telemetry_.add_sample_type(airdata_reduced_sample::PREFIX,
                             airdata_reduced_sample::create);
  telemetry_.add_sample_type(battery_sample::PREFIX, battery_sample::create);

  // This is really dumb.
  std::vector<std::string> file_types;
  file_types.emplace_back("airdata_reduced");
  file_types.emplace_back("battery");

  auto log_time = std::chrono::system_clock::now();

  for (const std::string &type : file_types) {
    std::ofstream *file = new std::ofstream;
    std::string filename = airball::format_log_filename(log_time, type);
    file->open(filename, std::ios::out);
    files_[type] = file;
  }
}

ESP32TelemetryClient::~ESP32TelemetryClient() {
  for (auto f = files_.begin(); f != files_.end(); ++f) {
    (*f).second->close();
  }
};

std::unique_ptr<sample> ESP32TelemetryClient::get() {
  while (true) {
    try {
      std::string data = reader_.read_line();
      uint8_t rssi = 0; // TODO: RSSI is useless for TCP
      auto current_time = std::chrono::system_clock::now();
      if (sample *s = telemetry_.parse(current_time, rssi, data)) {
        *files_[s->type()] << s->format().c_str() << std::endl;
        files_[s->type()]->flush();
        return std::unique_ptr<sample>(s);
      }
    } catch (const std::exception &e) { }
  }
}

} // namespace airball