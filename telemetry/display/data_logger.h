/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018, Ihab A.B. Awad
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
