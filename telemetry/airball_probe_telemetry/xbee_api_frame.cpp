#include "xbee_api_frame.h"

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

} // namespace airball