#include <sstream>
#include <iostream>
#include "xbee_api_frame.h"
#include "xbee_utils.h"

namespace airball {

uint8_t compute_checksum(const uint8_t api, const std::string& data) {
  uint16_t sum = api;
  for (const char& c : data) { sum += c; }
  return (uint8_t) 0xff - (uint8_t) (sum & 0xff);
}

xbee_api_frame::xbee_api_frame(const uint8_t api, const std::string& payload)
    : api_(api),
      payload_(payload),
      checksum_(compute_checksum(api, payload))
    {}

std::string xbee_api_frame::to_bytes() const {
  std::ostringstream os;
  os << (uint8_t) 0x7e; // Start
  xbee_utils::write_uint(os, payload_.length() + 1, 2);
  os << api_;
  os << payload_;
  os << checksum_;
  return os.str();
}

xbee_api_frame xbee_api_frame::from_bytes(
    std::function<void(char*, size_t)> read) {
  while (true) {
    for (char c; read(&c, 1), c != 0x7e; ) { }

    char x[2];
    read(x, 2);
    uint16_t length = (uint16_t) xbee_utils::interpret_uint(x, 2);

    char api;
    read(&api, 1);

    char payload_buffer[length - 1];
    read(payload_buffer, (size_t) length - 1);

    xbee_api_frame frame(api, std::string(payload_buffer, (size_t) length - 1));

    uint8_t checksum;
    read((char*) &checksum, 1);

    if (checksum == frame.checksum()) {
      return frame;
    }
  }
}

} // namespace airball