#include "xbee_known_types.h"

namespace airball {

xbee_known_types::xbee_type xbee_known_types::get_xbee_type(
    const std::string &athv) {
  if (athv == "174B") {
    // Legacy XBee S1
    return xbee_802_14;
  }
  if (athv == "184E") {
    // XBee Pro S1
    return xbee_802_14;
  }
  if (athv == "234A") {
    // XBee Pro S3B
    return xbee_900mhz;
  }
  if (athv == "2730") {
    // Xbee S6B 802.11
    return xbee_802_11;
  }
  return xbee_unknown;
}

} // namespace airball