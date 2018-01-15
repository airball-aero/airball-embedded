#include "data_source.h"

#include <ratio>
#include <thread>
#include <ostream>
#include <backward/strstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sstream>
#include <poll.h>
#include <regex>
#include "fake_data_sentence.h"

namespace airball {

class FakeDataSource : public DataSource {
public:
  FakeDataSource();
  virtual std::string next_data_sentence() override;
private:
  std::chrono::steady_clock::time_point start_;
};

constexpr static std::chrono::milliseconds kInterval(50);
constexpr static std::chrono::milliseconds kPeriod(10000);

FakeDataSource::FakeDataSource()
    : start_(std::chrono::steady_clock::now()) {}

std::string FakeDataSource::next_data_sentence() {
  std::this_thread::sleep_for(kInterval);
  auto time_delta = std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::steady_clock::now() - start_);
  double phase_ratio =
      (double)(time_delta.count() % kPeriod.count()) /
      (double)(kPeriod.count());
  return make_fake_data_sentence(phase_ratio);
}

class SerialDataSource : public DataSource {
public:
  SerialDataSource(const std::string& device);
  virtual std::string next_data_sentence() override;
private:
  int fd_;
};

SerialDataSource::SerialDataSource(const std::string& device) {
  std::cout << "SerialDataSource(" << device << ")" << std::endl;
  fd_ = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  std::cout << "fd = " << fd_ << std::endl;
  if (fd_ < 0) {
    perror("Unable to open specified device");
    return;
  }
  struct termios options;
  tcgetattr(fd_, &options);
  cfsetispeed(&options, B9600);
  cfsetospeed(&options, B9600);
  options.c_cflag |= (CLOCAL | CREAD);
  if (tcsetattr(fd_, TCSANOW, &options) < 0) {
    perror("Unable to set attributes on device");
    return;
  }
}

std::string SerialDataSource::next_data_sentence() {
  struct pollfd pfd;
  pfd.fd = fd_;
  pfd.events = POLLIN;
  std::ostringstream s;

  while (true) {
    int r = poll(&pfd, 1, -1);
    if (r <= 0) {
      perror("poll");
      continue;
    }
    char c;
    size_t n = read(fd_, &c, 1);
    if (c == '\n') {
      break;
    }
    if (n == 0) {
      continue;
    }
    s << c;
  }

  auto result = s.str();
  return result.erase(result.find_last_not_of(" \n\r\t") + 1);
}

DataSource* DataSource::NewSerialDataSource(const std::string& device) {
  return new SerialDataSource(device);
}

DataSource* DataSource::NewFakeDataSource() {
  return new FakeDataSource();
}

}  // namespace airball
