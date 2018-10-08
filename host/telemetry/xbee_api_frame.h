#ifndef AIRBALL_XBEE_API_FRAME_H
#define AIRBALL_XBEE_API_FRAME_H

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace airball {

/**
 * An xbee_api_frame is a frame that is written to or read from an XBee device
 * when in API mode. Such a frame contains a single byte API code, and a data
 * payload that is subject to interpretation by the reader or writer. All the
 * data is checksummed.
 */
class xbee_api_frame {
public:
  /**
   * Create an XBee API frame.
   *
   * @param api the API identifier that this frame represents.
   * @param payload the data payload for this API frame.
   */
  xbee_api_frame(uint8_t api, const std::string& payload);

  /**
   * Create an XBee API frame.
   *
   * @param api the API identifier that this frame represents.
   * @param payload the data payload for this API frame.
   */
  xbee_api_frame(uint8_t api, std::vector<char> payload)
    : xbee_api_frame(api, std::string(payload.data(), payload.size())) {}

  uint8_t api() const { return api_; }

  const std::string& payload() const { return payload_; }

  uint8_t checksum() const { return checksum_; };

  std::string to_bytes() const;

  static xbee_api_frame from_bytes(std::function<void(char*, size_t)> read);

private:
  const uint8_t api_;
  const std::string payload_;
  const uint8_t checksum_;
};

} // namespace airball

#endif // AIRBALL_XBEE_API_FRAME_H
