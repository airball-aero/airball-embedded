#include "xbee.h"

#include <asio/read.hpp>
#include <asio/read_until.hpp>
#include <asio/write.hpp>
#include <asio/impl/read.hpp>
#include <asio/impl/write.hpp>

namespace airball {

xbee::xbee(std::string serial_device_filename,
           unsigned int baud_rate) :
    device_filename(serial_device_filename),
    baud_rate(baud_rate),
    serial_port(io_service) {
  serial_port.open(serial_device_filename);
  serial_port.set_option(asio::serial_port_base::baud_rate(
      baud_rate));
  serial_port.set_option(asio::serial_port_base::character_size(
      8));
  serial_port.set_option(asio::serial_port_base::parity(
      asio::serial_port_base::parity::none));
  serial_port.set_option(asio::serial_port_base::stop_bits(
      asio::serial_port_base::stop_bits::one));
  serial_port.set_option(asio::serial_port_base::flow_control(
      asio::serial_port_base::flow_control::none));
}

void xbee::write(char c) {
  asio::write(serial_port, asio::buffer(&c, 1));
}

void xbee::write(const char *s, int len) {
  for (size_t i = 0; i < len; i++) {
    write(s[i]);
  }
}

void xbee::write(std::string str) {
  write(str.data(), str.length());
}

void xbee::write_uint8(uint8_t value) {
  write(value);
}

void xbee::write_uint16(uint16_t value) {
  write((uint8_t) ((value >> 8) & 0xff));   // MSB
  write((uint8_t) ((value     ) & 0xff));   // LSB
}

std::string xbee::read(uint16_t size) {
  char buf[size];
  memset(buf, 0, size);
  asio::read(
      serial_port,
      asio::buffer(buf, size),
      asio::transfer_all());
  return std::string(buf, size);
}

void xbee::discard_until(char c) {
  char cr;
  do {
    serial_port.read_some(asio::buffer(&cr, 1));
  } while (cr != c);
}

void xbee::discard_until(char *str) {
  int p = 0;
  size_t l = strlen(str) - 1;
  std::cout << "Trying to discard data" << std::endl;
  asio::error_code ec;
  do {
    char c;
    std::cout << "Trying to read" << std::endl;
    size_t len = serial_port.read_some(asio::buffer(&c, 1), ec);

    std::cout << "Got data to discard: " << len << ": " << c << std::endl;
    std::cout << "Error code: " << ec.value() << ", message: " << ec.message()
              << std::endl;

    if (str[p] == c) {
      std::cout << "Found " << c << " moving to next char " << str[p + 1]
                << std::endl;
      if (p == l) {
        std::cout << "Got a match" << std::endl;
        return;
      }
      p++;
    } else {
      std::cout << "No match" << std::endl;
      p = 0;
    }

  } while (ec.value() != asio::error::misc_errors::eof);
}

std::string xbee::get_line(const char end) {
  asio::streambuf input_data_buffer;
  std::istream input_buffer(&input_data_buffer);
  char line[512] = {0};
  asio::read_until(serial_port, input_data_buffer, end);
  input_buffer.getline(line, sizeof(line));
  return std::string(line);
}

std::string xbee::read_until(const char end) {
  asio::streambuf input_data_buffer;
  std::istream input_buffer(&input_data_buffer);
  char line[512] = {0};
  asio::read_until(serial_port, input_data_buffer, end);
  input_buffer.getline(line, sizeof(line));
  return std::string(line);
}

void xbee::write_api_frame(const xbee_api_frame& frame) {
  write_uint8(0x7e); // Start
  write_uint16((uint16_t)(frame.payload().length() + 1));
  write_uint8(frame.api());
  write(frame.payload());
  write_uint8(frame.checksum());
}

xbee_api_frame xbee::read_api_frame() {
  while (true) {
    // Synchronize with the beginning of an API packet.
    discard_until(API_FRAME_START_DELIMITER);

    uint8_t x[2];
    asio::read(
        serial_port,
        asio::buffer(x, 2),
        asio::transfer_all());
    uint16_t length = ((uint16_t) x[0] << 8) | (uint16_t) x[1];

    uint8_t api;
    asio::read(
        serial_port,
        asio::buffer(&api, 1),
        asio::transfer_all());

    char payload_buffer[length - 1];
    asio::read(
        serial_port,
        asio::buffer(payload_buffer, (size_t) length - 1),
        asio::transfer_all());

    xbee_api_frame frame(api, std::string(payload_buffer, (size_t) length - 1));

    uint8_t checksum;
    asio::read(
        serial_port,
        asio::buffer(&checksum, 1),
        asio::transfer_all());

    if (checksum == frame.checksum()) {
      return frame;
    }
  }
}

void xbee::enter_command_mode(unsigned int guard_time) {
  if (in_command_mode_) { return; }
  usleep(guard_time * 1000);
  write("+++");
  usleep(guard_time * 1000);
  read(3); // "OK\n"
  in_command_mode_ = true;
}

void xbee::send_command(std::string command) {
  write(command + "\r");
}

void xbee::exit_command_mode() {
  if (!in_command_mode_) { return; }
  send_command("ATCN");
  in_command_mode_ = false;
}

std::string xbee::get_hardware_version() {
  std::string result;
  ensure_command_mode([&]() {
    send_command("ATHV");
    result = read(5).substr(0, 4);
  });
  return result;
}

void xbee::ensure_command_mode(const std::function<void()> &f) {
  bool was_in_command_mode = in_command_mode_;
  if (!was_in_command_mode) { enter_command_mode(); }
  f();
  if (!was_in_command_mode) { exit_command_mode(); }
}

} // namespace airball