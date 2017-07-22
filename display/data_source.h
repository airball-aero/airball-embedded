#ifndef AIRBALL_DATA_SOURCE_H
#define AIRBALL_DATA_SOURCE_H

#include <string>

namespace airball {

/**
 * A source of raw sensor data from an Airball sensor.
 */
class DataSource {
public:

  virtual ~DataSource() {};

  /**
   * Blocks until the next data sentence is available, then returns it.
   *
   * TODO: Document data sentence format.
   *
   * @return a data sentence in standard format.
   */
  virtual std::string next_data_sentence() = 0;

  /**
   * Creates a new DataSource that returns data from a given serial device.
   *
   * @param device a serial device name.
   *
   * @return a DataSource.
   */
  static DataSource* NewSerialDataSource(const std::string& device);

  /**
   * Creates a new DataSource that returns fake data.
   *
   * @return a DataSource.
   */
  static DataSource* NewFakeDataSource();
};

}  // namespace airball

#endif //AIRBALL_DATA_SOURCE_H
