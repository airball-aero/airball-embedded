#include "xbee_api_payload.h"

#include <sstream>

namespace airball {

std::unique_ptr<xbee_api_receive> xbee_api_receive::interpret_frame(
    const xbee_api_frame &frame) {
  switch (frame.api()) {
    case 0x81:
      return std::make_unique<x81_receive_16_bit>(frame);
    case 0x88:
      return std::make_unique<x88_at_response>(frame);
    case 0x90:
      return std::make_unique<x90_receive_64_bit>(frame);
    default:
      return std::make_unique<xxx_unknown>(frame);
  }
}

x01_send_16_bit::x01_send_16_bit(
    const uint8_t frame_id,
    const uint16_t destination_address,
    const uint8_t options,
    const std::string& data) {
  std::ostringstream os;
  os << frame_id;
  xbee_utils::write_uint(os, destination_address, 2);
  os << options;
  os << data;
  frame_ = std::make_unique<xbee_api_frame>(0x01, os.str());
}

x08_at_command::x08_at_command(
    const uint8_t frame_id,
    const std::string& command,
    const std::string& value) {
  std::ostringstream os;
  os << frame_id;
  os << command.substr(0, 2);
  os << value;
  frame_ = std::make_unique<xbee_api_frame>(0x08, os.str());
}

x08_at_command::x08_at_command(
    const uint8_t frame_id,
    const std::string& command,
    const uint8_t value) {
  std::ostringstream os;
  os << frame_id;
  os << command.substr(0, 2);
  xbee_utils::write_uint(os, value, 1);
  frame_ = std::make_unique<xbee_api_frame>(0x08, os.str());
}

x08_at_command::x08_at_command(
    const uint8_t frame_id,
    const std::string& command,
    const uint16_t value) {
  std::ostringstream os;
  os << frame_id;
  os << command.substr(0, 2);
  xbee_utils::write_uint(os, value, 2);
  frame_ = std::make_unique<xbee_api_frame>(0x08, os.str());
}

x08_at_command::x08_at_command(
    const uint8_t frame_id,
    const std::string& command,
    const uint64_t value) {
  std::ostringstream os;
  os << frame_id;
  os << command.substr(0, 2);
  xbee_utils::write_uint(os, value, 8);
  frame_ = std::make_unique<xbee_api_frame>(0x08, os.str());
}

x10_send_64_bit::x10_send_64_bit(
    const uint8_t frame_id,
    const uint64_t destination_address_64_bit,
    const uint16_t destination_address_16_bit,
    const uint8_t broadcast_radius,
    const uint8_t options,
    const std::string& data) {
  std::ostringstream os;
  os << frame_id;
  xbee_utils::write_uint(os, destination_address_64_bit, 8);
  xbee_utils::write_uint(os, destination_address_16_bit, 2);
  os << broadcast_radius;
  os << options;
  os << data;
  frame_ = std::make_unique<xbee_api_frame>(0x10, os.str());
}

} // namespace airball
