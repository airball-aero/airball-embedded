#include "xbee_telemetry_client.h"

#include <fstream>

#include "airdata_sample.h"
#include "battery_sample.h"
#include "format.h"

namespace airball {

XbeeTelemetryClient::XbeeTelemetryClient(
    const std::string& serial_device_filename)
    : serial_device_filename_(serial_device_filename),
      radio_(serial_device_filename_, 9600) {

  radio_.enterCommandMode();

  radio_.sendCommand("ATNIAIRBALL_BASE");
  radio_.sendCommand("ATID=5555");
  radio_.sendCommand("ATMY=8888");
  radio_.sendCommand("ATAP=1");

  radio_.exitCommandMode();

  telemetry_.add_sample_type(airdata_sample::PREFIX, airdata_sample::create);
  telemetry_.add_sample_type(battery_sample::PREFIX, battery_sample::create);

  // This is really dumb.
  std::vector<std::string> file_types;
  file_types.emplace_back("airdata");
  file_types.emplace_back("battery");

  stats_["unusable"] = 0;

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
  xbee_packet packet = radio_.read_packet();
  while (true) {
    auto current_time = std::chrono::system_clock::now();

    if (packet.frame_type == xbee_packet::PacketTypeReceive16Bit) {
      xbee_packet::PacketReceive16Bit data(packet.frame_data);

      if (sample *s = telemetry_.parse(current_time, data.rssi(), data.data())) {
        stats_[s->type()]++;
        *files_[s->type()] << s->format().c_str() << std::endl;
        return std::unique_ptr<sample>(s);
      } else {
        stats_["unusable"]++;
      }
    }
  }
}

} // namespace airball