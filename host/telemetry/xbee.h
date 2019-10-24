#ifndef AIRBALL_XBEE_H
#define AIRBALL_XBEE_H

#include <string>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/io_service.hpp>
#include <istream>
#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <condition_variable>
#include "xbee_api_frame.h"

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

  ~xbee();

  // RAW DATA WRITE HELPERS

  // Write a number of different data types
  void write(char c);
  void write(const char *s, int len);
  void write(std::string str);

  // RAW DATA READ HELPERS

  // Gets a contiguous line of text
  std::string get_line(const char end = '\n');

  // XBEE CONFIGURATION HELPERS

  // Perform work in command mode, where serial messages are sent directly over
  // the serial link to the XBee to configure it rather than broadcast over the
  // XBee network to other XBees.
  void enter_command_mode(unsigned int guard_time = 1200);
  void send_command(std::string command);
  void exit_command_mode();

private:
  void ensure_command_mode(const std::function<void()> &f);

  const std::string device_filename;
  const unsigned int baud_rate;

  boost::asio::streambuf streambuf_;
  boost::asio::io_service io_service_;
  boost::asio::serial_port serial_port_;
  bool in_command_mode_ = false;
};

} // namespace airball

#endif // AIRBALL_XBEE_H
