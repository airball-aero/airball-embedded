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

#include "data_logger.h"

#include <fstream>
#include <iostream>
#include <memory>

namespace airball {

class FakeDataLogger : public DataLogger {
public:
  FakeDataLogger() {}
  virtual ~FakeDataLogger() {}
  virtual void log(const std::string& line) {}
  virtual void flush() {}
};

class FileDataLogger : public DataLogger {
public:
  FileDataLogger(const std::string& path);
  virtual ~FileDataLogger();
  virtual void log(const std::string& line);
  virtual void flush();
private:
  std::unique_ptr<std::ofstream> file_;
};

FileDataLogger::FileDataLogger(const std::string &path) {
  file_.reset(new std::ofstream);
  file_->open(path, std::ios::out | std::ios::app);
}

FileDataLogger::~FileDataLogger() {
  flush();
  file_->close();
}

void FileDataLogger::log(const std::string &line) {
  *file_ << line << std::endl;
}

void FileDataLogger::flush() {
  file_->flush();
}

DataLogger* DataLogger::NewFileDataLogger(const std::string& path) {
  return new FileDataLogger(path);
}

DataLogger* DataLogger::NewFakeDataLogger() {
  return new FakeDataLogger();
}

}  // namespace airball
