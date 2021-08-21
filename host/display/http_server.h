#ifndef AIRBALL_HTTP_SERVER_H
#define AIRBALL_HTTP_SERVER_H

#include <memory>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <string>
#include <thread>

namespace airball {

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class http_resource {
public:
  http_resource();
  virtual ~http_resource();

  http::response <http::string_body>
  handle_request(http::request <http::string_body> request);

  virtual http::response <http::string_body>
  head(http::request <http::string_body> request);

  virtual http::response <http::string_body>
  get(http::request <http::string_body> request);

  virtual http::response <http::string_body>
  post(http::request <http::string_body> request);
};

class http_server {
public:
  http_server(std::string address, unsigned short port);

  ~http_server();

  void add_resource(std::string path, std::unique_ptr <http_resource> resource);

  void run();

private:
  bool running_;
  std::string address_;
  unsigned short port_;
  std::map <std::string, std::unique_ptr<http_resource>> resources_;
};

} // namespace airball

#endif // AIRBALL_HTTP_SERVER_H
