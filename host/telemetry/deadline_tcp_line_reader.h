#ifndef AIRBALL_TELEMETRY_DEADLINE_TCP_LINE_READER_H
#define AIRBALL_TELEMETRY_DEADLINE_TCP_LINE_READER_H

#include <boost/asio/connect.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/write.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

// Reads lines from a TCP server, implementing a deadline and attempting to
// reconnect until a connection is re-established.
class deadline_tcp_line_reader {
public:
  deadline_tcp_line_reader(std::string host, std::string port,
                           boost::posix_time::time_duration timeout);

  // Reads a line from the TCP server. Upon deadline expiry, returns the empty
  // string to un-block the caller. Call again to re-attempt connection and
  // return the next available data.
  std::string read_line();

private:
  void connect();
  void check_deadline();

  const std::string host_;
  const std::string port_;
  const boost::posix_time::time_duration timeout_;
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::deadline_timer deadline_;
  boost::asio::streambuf input_buffer_;
};

#endif //AIRBALL_TELEMETRY_DEADLINE_TCP_LINE_READER_H
