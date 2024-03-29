#ifndef AIRBALL_XBEE_API_PAYLOAD_H
#define AIRBALL_XBEE_API_PAYLOAD_H

#include <vector>
#include <string>
#include <memory>
#include "xbee_api_frame.h"
#include "xbee_utils.h"

namespace airball {

/**
 * An XBee API payload is a wrapper around an XBee API frame that interprets the
 * contents according to the API code in the frame.
 */
class xbee_api_payload {
public:
  virtual const xbee_api_frame& frame() const = 0;
};

/**
 * A payload that results from reception of data from the XBee. The frame
 * contents need to be interpreted.
 */
class xbee_api_receive : public xbee_api_payload {
public:
  const xbee_api_frame& frame() const override { return frame_; }

  // Interprets the supplied API frame as an API payload, and returns the
  // result. This method contains a registry of all known API payloads.
  static std::unique_ptr<xbee_api_receive> interpret_frame(
      const xbee_api_frame &frame);

protected:
  explicit xbee_api_receive(const xbee_api_frame& frame) : frame_(frame) {}

private:
  const xbee_api_frame frame_;
};

/**
 * An unrecognized payload (generally meaning a frame was received, but the API
 * ID is not recognized by the current version of this software).
 */
class xxx_unknown : public xbee_api_receive {
public:
  explicit xxx_unknown(const xbee_api_frame& frame)
      : xbee_api_receive(frame) {}
};

/**
 * Received network message with 16-bit addressing.
 */
class x81_receive_16_bit : public xbee_api_receive {
public:
  explicit x81_receive_16_bit(const xbee_api_frame& frame)
      : xbee_api_receive(frame) {}

  uint16_t source_address() const {
    return (uint16_t) xbee_utils::interpret_uint(&frame().payload()[0], 2);
  }

  uint8_t rssi() const {
    return (uint8_t) frame().payload()[2];
  }

  uint8_t options() const {
    return (uint8_t) frame().payload()[3];
  }

  std::string data() {
    return std::string(
        frame().payload().data() + 4,
        frame().payload().length() - 4);
  };
};

/**
 * Response from an AT command.
 */
class x88_at_response : public xbee_api_receive {
public:
  explicit x88_at_response(const xbee_api_frame& frame)
      : xbee_api_receive(frame) {}

  uint8_t frame_id() const {
    return (uint8_t) frame().payload()[0];
  }

  std::string command() const {
    return std::string(frame().payload().data() + 1, 2);
  }

  uint8_t status() const {
    return (uint8_t) frame().payload()[3];
  }

  std::string data() {
    return std::string(data_start(), data_length());
  };

  uint8_t data_as_uint8_t() {
    if (data_length() < 1) { return 0; }
    return (uint8_t) xbee_utils::interpret_uint(data_start(), 1);
  }

  uint16_t data_as_uint16_t() {
    if (data_length() < 2) { return 0; }
    return (uint16_t) xbee_utils::interpret_uint(data_start(), 2);
  }

  uint64_t data_as_uint64_t() {
    if (data_length() < 8) { return 0; }
    return xbee_utils::interpret_uint(data_start(), 8);
  }

private:
  const char* data_start() const { return frame().payload().data() + 4; }
  size_t data_length() const { return frame().payload().length() - 4; }
};

/**
 * Received network message with 64-bit addressing.
 */
class x90_receive_64_bit : public xbee_api_receive {
public:
  explicit x90_receive_64_bit(const xbee_api_frame& frame)
      : xbee_api_receive(frame) {}

  uint64_t source_address_64_bit() const {
    return xbee_utils::interpret_uint(&frame().payload()[0], 8);
  }

  uint64_t source_address_16_bit() const {
    return xbee_utils::interpret_uint(&frame().payload()[8], 2);
  }

  uint8_t options() const {
    return (uint8_t) frame().payload()[10];
  }

  std::string data() {
    return std::string(
        frame().payload().data() + 11,
        frame().payload().length() - 11);
  };
};

/**
 * A payload that is to be sent to the XBee. The constructor is expected to take
 * individual meaningful pieces of data and construct the enclosed frame.
 */
class xbee_api_send : public xbee_api_payload {
public:
  const xbee_api_frame& frame() const override { return *frame_; }

protected:
  std::unique_ptr<xbee_api_frame> frame_;
};

/**
 * A request to send data with 16-bit addressing.
 */
class x01_send_16_bit : public xbee_api_send {
public:
  x01_send_16_bit(
      uint8_t frame_id,
      uint16_t destination_address,
      uint8_t options,
      const std::string& data);
};

/**
 * An AT command represented as an API frame.
 */
class x08_at_command : public xbee_api_send {
public:
  x08_at_command(
      uint8_t frame_id,
      const std::string& command,
      const std::string& value);

  x08_at_command(
      uint8_t frame_id,
      const std::string& command,
      uint8_t value);

  x08_at_command(
      uint8_t frame_id,
      const std::string& command,
      uint16_t value);

  x08_at_command(
      uint8_t frame_id,
      const std::string& command,
      uint64_t value);
};

/**
 * A request to send data with 64-bit addressing.
 */
class x10_send_64_bit : public xbee_api_send {
public:
  x10_send_64_bit(
      uint8_t frame_id,
      uint64_t destination_address_64_bit,
      uint16_t destination_address_16_bit,
      uint8_t broadcast_radius,
      uint8_t options,
      const std::string& data);
};

} // namespace airball

#endif // AIRBALL_XBEE_API_PAYLOAD_H
