#include <sstream>
#include <iostream>
#include "xbee_known_types.h"

namespace airball {

xbee_known_types::xbee_type xbee_known_types::get_xbee_type(
    uint16_t athv) {
  if (athv == 0x174B) {
    // Legacy XBee S1
    return xbee_802_14;
  }
  if (athv == 0x184E) {
    // XBee Pro S1
    return xbee_802_14;
  }
  if (athv == 0x234A) {
    // XBee Pro S3B
    return xbee_900mhz;
  }
  if (athv == 0x2730) {
    // Xbee S6B 802.11
    return xbee_802_11;
  }
  return xbee_unknown;
}

xbee_known_types::xbee_type xbee_known_types::get_xbee_type(
    const std::string &athv) {
  return get_xbee_type(std::stoi(athv, nullptr, 16));
}

} // namespace airball