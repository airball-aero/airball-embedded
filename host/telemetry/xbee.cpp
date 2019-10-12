#include "xbee.h"

#include <thread>
#include <strstream>
#include <asio/impl/read.hpp>
#include <asio/impl/write.hpp>

namespace airball {

xbee::xbee(std::string serial_device_filename,
           unsigned int baud_rate) :
    device_filename(serial_device_filename),
    baud_rate(baud_rate),
    serial_port_(io_service_) {
  serial_port_.open(serial_device_filename);
  serial_port_.set_option(asio::serial_port_base::baud_rate(
      baud_rate));
  serial_port_.set_option(asio::serial_port_base::character_size(
      8));
  serial_port_.set_option(asio::serial_port_base::parity(
      asio::serial_port_base::parity::none));
  serial_port_.set_option(asio::serial_port_base::stop_bits(
      asio::serial_port_base::stop_bits::one));
  serial_port_.set_option(asio::serial_port_base::flow_control(
      asio::serial_port_base::flow_control::none));

  reader_ = std::thread([&]() {
    while (reading_) {
      char buf = 0;
      {
        std::unique_lock<std::mutex> lock(serial_port_mutex_);
        asio::read(
            serial_port_,
            asio::buffer(&buf, 1),
            asio::transfer_all());
      }
      {
        std::unique_lock<std::mutex> lock(buffer_mutex_);
        buffer_.emplace_back(buf);
      }
      cv_.notify_one();
    }
  });
}

xbee::~xbee() {
  reading_ = false;
  reader_.join();
}

void xbee::write(char c) {
  std::unique_lock<std::mutex> lock(serial_port_mutex_);
  asio::write(serial_port_, asio::buffer(&c, 1));
}

void xbee::write(const char *s, int len) {
  for (size_t i = 0; i < len; i++) {
    write(s[i]);
  }
}

void xbee::write(std::string str) {
  write(str.data(), str.length());
}

char xbee::read() {
  std::unique_lock<std::mutex> lock(buffer_mutex_);
  cv_.wait(lock, [&](){ return !buffer_.empty(); });
  char c = buffer_.front();
  buffer_.pop_front();
  return c;
}

std::string xbee::get_line(const char end) {
  std::vector<char> line;
  for (;;) {
    char c = read();
    if (c == end) {
      return std::string(line.begin(), line.end());
    }
    line.push_back(c);
  }
}

void xbee::enter_command_mode(unsigned int guard_time) {
  if (in_command_mode_) { return; }
  usleep(guard_time * 1000);
  write("+++");
  usleep(guard_time * 1000);
  in_command_mode_ = true;
}

void xbee::send_command(std::string command) {
  ensure_command_mode([&]() {
    write(command + "\r");
  });
}

void xbee::exit_command_mode() {
  if (!in_command_mode_) { return; }
  send_command("ATCN");
  in_command_mode_ = false;
}

void xbee::ensure_command_mode(const std::function<void()> &f) {
  bool was_in_command_mode = in_command_mode_;
  if (!was_in_command_mode) { enter_command_mode(); }
  f();
  if (!was_in_command_mode) { exit_command_mode(); }
}

} // namespace airball