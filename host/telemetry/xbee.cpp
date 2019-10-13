#include "xbee.h"

#include <thread>
#include <strstream>
#include <asio/impl/read.hpp>
#include <asio/impl/write.hpp>
#include <asio/read_until.hpp>

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
}

xbee::~xbee() {}

void xbee::write(char c) {
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

std::string xbee::get_line(const char end) {
  asio::read_until(serial_port_, streambuf_, "\r\n");
  std::istream is(&streambuf_);
  std::string line;
  std::getline(is, line);
  return line;
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