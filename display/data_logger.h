#ifndef AIRBALL_DATA_LOGGER_H
#define AIRBALL_DATA_LOGGER_H

#include <string>

namespace airball {

/**
 * A source of raw sensor data from an Airball sensor.
 */
class DataLogger {
public:

  virtual ~DataLogger() {};

  /**
   * Write a line of data to the log.
   */
  virtual void log(const std::string& line) = 0;

  /**
   * Ensure all logged entries so far are flushed to stable storage.
   */
  virtual void flush() = 0;

  /**
   * Creates a no-op DataLogger.
   *
   * @return a DataLogger.
   */
  static DataLogger* NewFakeDataLogger();

  /**
   * Creates a new DataLogger that writes to a given filesystem path.
   *
   * @return a DataLogger.
   */
  static DataLogger* NewFileDataLogger(const std::string& path);
};

}  // namespace airball

#endif //AIRBALL_DATA_LOGGER_H
