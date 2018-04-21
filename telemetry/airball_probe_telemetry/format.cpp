#include "format.h"

namespace airball {

std::string format_log_filename(
    std::chrono::time_point<std::chrono::system_clock> time_point,
    const std::string &prefix) {
  char time_buf[200];
  std::time_t t = std::chrono::system_clock::to_time_t(time_point);
  strftime(time_buf, sizeof(time_buf), "%Y%m%d_%H%M%S", std::gmtime(&t));

  return "log/" + prefix + "_" + std::string(time_buf) + ".log";
}

std::string format_time(
    std::chrono::time_point<std::chrono::system_clock> time_point) {
  char time_buf[200];
  std::time_t t = std::chrono::system_clock::to_time_t(time_point);
  strftime(time_buf, sizeof(time_buf), "%F %T", std::gmtime(&t));

  return std::string(time_buf);
}

}
