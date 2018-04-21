#ifndef AIRBALL_TELEMETRY_FORMAT_H
#define AIRBALL_TELEMETRY_FORMAT_H

#include <string>
#include <chrono>

namespace  airball {

/**
 * Create a log file name for a given situation.
 *
 * @param time_point the time at which the log is started.
 * @param prefix an alphanumeric prefix identifying the "type of information"
 *     to be stored in the log file.
 * @return a log file name following a standard convention.
 */
std::string format_log_filename(
    std::chrono::time_point<std::chrono::system_clock> time_point,
    const std::string &prefix);

/**
 * Create a string form of a wall clock time.
 *
 * @param time_point a time point.
 * @return a human readable string form.
 */
std::string format_time(
    std::chrono::time_point<std::chrono::system_clock> time_point);

}  // namespace airball

#endif //AIRBALL_TELEMETRY_FORMAT_H
