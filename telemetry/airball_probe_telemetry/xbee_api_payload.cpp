#include "xbee_api_payload.h"

#include <sstream>

namespace airball {

std::unique_ptr<xbee_api_receive> xbee_api_receive::interpret_frame(
    const xbee_api_frame &frame) {
  switch (frame.api()) {
    case 0x81:
      return std::make_unique<x81_receive_16_bit>(frame);
    default:
      return std::make_unique<xxx_unknown>(frame);
  }
}

void xbee_api_send::write_uint16(std::ostream& os, const uint16_t value) {
  os << ((uint8_t) ((value >> 8) & 0xff));   // MSB
  os << ((uint8_t) ((value)      & 0xff));   // LSB
}

x01_send_16_bit::x01_send_16_bit(
    const uint8_t frame_id,
    const uint16_t destination_address,
    const uint8_t options,
    const std::string data) {
  std::ostringstream os;
  os << frame_id;
  write_uint16(os, destination_address);
  os << options;
  os << data;
  frame_ = std::make_unique<xbee_api_frame>(0x01, os.str());
}

} // namespace airball
