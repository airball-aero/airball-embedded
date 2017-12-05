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
