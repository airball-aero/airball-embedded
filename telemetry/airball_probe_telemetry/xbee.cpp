#include "xbee.h"

#include <asio/read.hpp>
#include <asio/read_until.hpp>
#include <asio/write.hpp>
#include <asio/impl/read.hpp>
#include <asio/impl/write.hpp>

namespace airball {

xbee::xbee(std::string serial_device_filename,
           unsigned int baud_rate,
           xbee_node_type node_type) :
    device_filename(serial_device_filename),
    baud_rate(baud_rate),
    node_type(node_type),
    serial_port(io_service),
    input_buffer(&input_data_buffer),
    output_buffer(&output_data_buffer) {
  serial_port.open(serial_device_filename);
  serial_port.set_option(asio::serial_port_base::baud_rate(baud_rate));
  serial_port.set_option(asio::serial_port_base::character_size(8));
  serial_port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
  serial_port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
  serial_port.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
}

void xbee::initialize() {
  enterCommandMode();
  switch (node_type) {
    case BASE_STATION:
      sendCommand("ATNIAIRBALL_BASE");
      sendCommand("ATID=5555");
      sendCommand("ATMY=8888");
      sendCommand("ATAP=1");
      break;
    case DATA_SOURCE:
      sendCommand("ATNIAIRBALL_PROBE");
      sendCommand("ATID=5555");
      sendCommand("ATMY=7777");
      sendCommand("ATSM=0");
      sendCommand("ATSP=64");
      sendCommand("ATDL=8888");
      sendCommand("ATAP=1");
      break;
  }
  exitCommandMode();
}

void xbee::send(const std::string &buf) {
  write_uint8(0x7e); // Start
  write_uint16(buf.length() + 1 + 1 + 2 + 1);

  reset_checksum();
  write_uint8(0x01); // API ID
  write_uint8(0x00); // Frame ID
  switch (node_type) {
    case BASE_STATION:
      write_uint16(0x7777); // Destination Address
      break;
    case DATA_SOURCE:
      write_uint16(0x8888); // Destination Address
      break;
  }
  write_uint8(0x00); // Options
  write(buf.c_str(), buf.length()); // Data
  write_checksum(); // Checksum
}

xbee_packet xbee::receive() {
  // Synchronize with the beginning of an API packet.
  discard_until(START_DELIMITER);

  xbee_packet packet;

  uint8_t x[2];
  asio::read(serial_port, asio::buffer(x, 2), asio::transfer_all());

  packet.length = ((uint16_t) x[0] << 8) | (uint16_t) x[1];

  packet.frame_data.resize(packet.length - 1);
  asio::read(serial_port, asio::buffer(&packet.frame_type, 1),
             asio::transfer_all());
  asio::read(serial_port, asio::buffer(packet.frame_data.data(),
                                       (size_t) packet.length - 1),
             asio::transfer_all());
  asio::read(serial_port, asio::buffer(&packet.checksum, 1),
             asio::transfer_all());

  return packet;
}

void xbee::write_unchecked(std::string str) {
  output_buffer << str;
  asio::write(serial_port, output_data_buffer);
}

void xbee::write(char c) {
  write_checksum_sum += c;
  asio::write(serial_port, asio::buffer(&c, 1));
}

void xbee::write_checksum() {
  write(0xff - (uint8_t) (write_checksum_sum & 0xff));
}

void xbee::reset_checksum() {
  write_checksum_sum = 0;
}

void xbee::write(const char *s, int len) {
  for (size_t i = 0; i < len; i++) {
    write(s[i]);
  }
}

void xbee::write_uint8(uint8_t value) {
  write(value);
}

void xbee::write_uint16(uint16_t value) {
  write((uint8_t) (value & 0xff00 >> 8));   // MSB
  write((uint8_t) (value & 0x00ff));        // LSB
}

void xbee::discard_until(char c) {
  char cr;
  do {
    serial_port.read_some(asio::buffer(&cr, 1));
  } while (cr != c);
}

void xbee::discard_until(char *str) {
  int p = 0;
  int l = strlen(str) - 1;
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
  char line[512] = {0};
  asio::read_until(serial_port, input_data_buffer, end);
  input_buffer.getline(line, sizeof(line));
  std::cout << "Got line: " << line << std::endl;
  return std::string(line);
}

std::string xbee::read_until(const char end) {
  char line[512] = {0};
  asio::read_until(serial_port, input_data_buffer, end);
  input_buffer.getline(line, sizeof(line));
  std::cout << "Got line: " << line << std::endl;
  return std::string(line);
}

void xbee::enterCommandMode(unsigned int guard_time) {
  usleep(guard_time * 1000);
  write_unchecked("+++");
  usleep(guard_time * 1000);
}

void xbee::sendCommand(std::string command) {
  write_unchecked(command + "\r");
}

void xbee::exitCommandMode() {
  sendCommand("ATCN");
}

} // namespace airball