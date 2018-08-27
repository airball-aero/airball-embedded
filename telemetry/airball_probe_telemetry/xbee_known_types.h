#ifndef AIRBALL_XBEE_KNOWN_TYPES_H
#define AIRBALL_XBEE_KNOWN_TYPES_H

#include <string>

namespace airball {

class xbee_known_types {
public:
  enum xbee_type {
    xbee_unknown,
    xbee_802_11,
    xbee_802_14,
    xbee_900mhz,
  };

  static xbee_type get_xbee_type(const std::string &athv);
};

} // namespace airball

#endif // AIRBALL_XBEE_KNOWN_TYPES_H
