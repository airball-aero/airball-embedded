#ifndef AIRBALL_XBEE_H
#define AIRBALL_XBEE_H

#include <string>
#include <asio/serial_port.hpp>
#include <asio/streambuf.hpp>
#include <istream>
#include <iostream>
#include "xbee_packet.h"

namespace airball {

/**
 * Represents a connection to an XBee wireless communication module made via a
 * serial connection on the current host. Encapsulates control of the XBee
 * module itself as well as communication to and from other nodes on the XBee
 * network.
 */
class xbee {
public:
  /**
   * Create a new XBee connection.
   *
   * @param serial_device_filename the filename of the serial device on the
   *     current host for connecting to the XBee module.
   * @param baud_rate the baud rate of the serial connection to be made.
   */
  xbee(std::string serial_device_filename,
      unsigned int baud_rate);

  // DATA WRITE HELPERS

  // Write a string without checksumming
  void write_unchecked(std::string str);

  // Methods to manage the running write checksum
  void write_checksum();
  void reset_checksum();

  // Write a number of different data types, all with checksumming
  void write(char c);
  void write(const char *s, int len);
  void write_uint8(uint8_t value);
  void write_uint16(uint16_t value);

  // Send a packet to a given address.
  void send_packet(const uint16_t destination, const std::string &buf);

  // DATA READ HELPERS

  // Discard inputs until a given input is seen
  void discard_until(char c);
  void discard_until(char *str);

  // Gets a contiguous line of text
  std::string get_line(const char end = '\n');

  // Reads inputs until a given input is seen
  std::string read_until(const char end = 0x7e);

  // Reads the next available packet.
  xbee_packet read_packet();

  // XBEE CONFIGURATION HELPERS

  // Perform work in command mode, where serial messages are sent directly over
  // the serial link to the XBee to configure it rather than broadcast over the
  // XBee network to other XBees.
  void enterCommandMode(unsigned int guard_time = 1200);
  void sendCommand(std::string command);
  void exitCommandMode();

private:
  const std::string device_filename;
  const unsigned int baud_rate;

  asio::io_service io_service;
  asio::serial_port serial_port;
  asio::streambuf input_data_buffer, output_data_buffer;
  std::istream input_buffer;
  std::ostream output_buffer;
  uint16_t write_checksum_sum = 0;

  static const char START_DELIMITER = 0x7e;
};

} // namespace airball

#endif // AIRBALL_XBEE_H
