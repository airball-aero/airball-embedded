#include "deadline_tcp_line_reader.h"

deadline_tcp_line_reader::deadline_tcp_line_reader(std::string host,
                                                   std::string port,
                                                   boost::posix_time::time_duration timeout)
    : host_(host), port_(port), timeout_(timeout),
      socket_(io_service_),
      deadline_(io_service_) {
  deadline_.expires_at(boost::posix_time::pos_infin);
  check_deadline();
}

void deadline_tcp_line_reader::connect() {
  try {
    boost::asio::ip::tcp::resolver::query query(host_, port_);
    boost::asio::ip::tcp::resolver::iterator iter = boost::asio::ip::tcp::resolver(
        io_service_).resolve(query);
    deadline_.expires_from_now(timeout_);
    boost::system::error_code ec = boost::asio::error::would_block;
    boost::asio::async_connect(socket_, iter,
                               boost::lambda::var(ec) = boost::lambda::_1);
    do io_service_.run_one(); while (ec == boost::asio::error::would_block);
  } catch (std::exception e) {}
}

std::string deadline_tcp_line_reader::read_line() {
  if (!socket_.is_open()) connect();
  if (socket_.is_open()) {
    const boost::posix_time::seconds timeout(1);
    deadline_.expires_from_now(timeout);
    boost::system::error_code ec = boost::asio::error::would_block;
    boost::asio::async_read_until(socket_, input_buffer_, '\n',
                                  boost::lambda::var(ec) = boost::lambda::_1);
    do io_service_.run_one(); while (ec == boost::asio::error::would_block);
    if (!ec) {
      std::string line;
      std::istream is(&input_buffer_);
      std::getline(is, line);
      return line;
    }
  }
  return "";
}

void deadline_tcp_line_reader::check_deadline() {
  if (deadline_.expires_at() <=
      boost::asio::deadline_timer::traits_type::now()) {
    boost::system::error_code ignored_ec;
    socket_.close(ignored_ec);
    deadline_.expires_at(boost::posix_time::pos_infin);
  }
  deadline_.async_wait(
      boost::lambda::bind(&deadline_tcp_line_reader::check_deadline, this));
}
