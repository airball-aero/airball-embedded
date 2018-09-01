#ifndef AIRBALL_XBEE_KNOWN_TYPES_H
#define AIRBALL_XBEE_KNOWN_TYPES_H

#include <string>

namespace airball {

class xbee_known_types {
public:
  enum xbee_type {
    xbee_unknown = -1,
    xbee_802_14 = 0,
    xbee_900mhz = 1,
    xbee_802_11 = 2,
  };

  static xbee_type get_xbee_type(uint16_t athv);
  static xbee_type get_xbee_type(const std::string &athv);
};

} // namespace airball

#endif // AIRBALL_XBEE_KNOWN_TYPES_H
