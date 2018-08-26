#ifndef AIRBALL_XBEE_API_PAYLOAD_H
#define AIRBALL_XBEE_API_PAYLOAD_H

#include <vector>
#include <string>
#include <memory>
#include "xbee_api_frame.h"

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
    return
        ((uint16_t) frame().payload()[0] << 8) |
        ((uint16_t) frame().payload()[1]);
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
 * A payload that is to be sent to the XBee. The constructor is expected to take
 * individual meaningful pieces of data and construct the enclosed frame.
 */
class xbee_api_send : public xbee_api_payload {
public:
  const xbee_api_frame& frame() const override { return *frame_; }

protected:
  std::unique_ptr<xbee_api_frame> frame_;
  // TODO(ihab): "write()" functions duplicated here and xbee.cpp; code smell
  void write_uint16(std::ostream& os, uint16_t value);
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
      std::string data);
};

} // namespace airball

#endif // AIRBALL_XBEE_API_PAYLOAD_H
