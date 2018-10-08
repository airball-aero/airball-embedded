#include <sstream>
#include <iomanip>
#include "xbee_utils.h"

namespace airball {

uint64_t xbee_utils::interpret_uint(const void *data, int bytes) {
  uint64_t accumulator = 0;
  for (int shift = 0; bytes-- > 0; shift++) {
    accumulator |= ((const uint8_t*) data)[bytes] << (8 * shift);
  }
  return accumulator;
}

void xbee_utils::write_uint(std::ostream& os, const uint64_t data, int bytes) {
  while (bytes-- > 0) {
    os << (uint8_t) ((data >> bytes * 8) & 0xff);
  }
}

static std::string bytes_to_string(const std::string& bytes) {
  std::ostringstream os;
  for (char c : bytes) {
    os << std::hex << std::setfill('0') << std::setw(2)
       << +((uint8_t) c) << " ";
  }
  return os.str();
}

} // namespace airball