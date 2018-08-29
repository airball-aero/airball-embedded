#ifndef AIRBALL_XBEE_UTILS_H
#define AIRBALL_XBEE_UTILS_H

#include <cstdint>
#include <ostream>

namespace airball {

class xbee_utils {
public:
  static uint64_t interpret_uint(const void *data, int bytes);
  static void write_uint(std::ostream& os, uint64_t data, int bytes);
  static std::string bytes_to_string(const std::string& bytes);
};

} // namespace airball

#endif // AIRBALL_TELEMETRY_XBEE_UTILS_H
