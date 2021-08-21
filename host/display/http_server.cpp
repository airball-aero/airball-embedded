#include <iostream>
#include "http_server.h"

namespace airball {

http_resource::http_resource() {}

http_resource::~http_resource() {}

http::response<http::string_body>
http_resource::handle_request(http::request<http::string_body> request) {
  http::response<http::string_body> response;
  switch (request.method()) {
    case boost::beast::http::verb::head:
      response = this->head(request);
      break;
    case boost::beast::http::verb::get:
      response = this->get(request);
      break;
    case boost::beast::http::verb::post:
      response = this->post(request);
      break;
    default:
      response = {http::status::bad_request,
                  request.version()};
      response.set(http::field::content_type, "text/html");
      response.body() = std::string("unknown verb");
      break;
  }
  response.keep_alive(request.keep_alive());
  response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  response.prepare_payload();
  return response;
}

http::response<http::string_body>
http_resource::head(http::request<http::string_body> request) {
  http::response<http::string_body> response = {http::status::bad_request,
                                                request.version()};
  response.set(http::field::content_type, "text/html");
  response.body() = std::string("HEAD not implemented");
  return response;
}

http::response<http::string_body>
http_resource::get(http::request<http::string_body> request) {
  http::response<http::string_body> response = {http::status::bad_request,
                                                request.version()};
  response.set(http::field::content_type, "text/html");
  response.body() = std::string("GET not implemented");
  return response;
}

http::response<http::string_body>
http_resource::post(http::request<http::string_body> request) {
  http::response<http::string_body> response = {http::status::bad_request,
                                                request.version()};
  response.set(http::field::content_type, "text/html");
  response.body() = std::string("POST not implemented");
  return response;
}

http_server::http_server(std::string address, unsigned short port)
    : running_(false), address_(address), port_(port) {}

http_server::~http_server() {}

class socket_handler {
public:
  socket_handler(tcp::socket &socket,
                 std::map<std::string, std::unique_ptr<http_resource>>& resources)
      : socket_(socket), resources_(resources) {}

  void handle() {
    beast::flat_buffer buffer;

    for (;;) {
      http::request<http::string_body> req;
      beast::error_code read_err;
      http::read(socket_, buffer, req, read_err);
      if (read_err == http::error::end_of_stream) {
        break;
      }
      if (read_err) {
        return;
      }
      if (handle_request(std::move(req))) {
        break;
      } else {
      }
    }

    beast::error_code shutdown_error;
    socket_.shutdown(tcp::socket::shutdown_send, shutdown_error);
  }

private:
  socket_handler(const socket_handler& sh);

  template<bool isRequest, class Body, class Fields>
  void send(http::request<http::string_body> &request,
            http::message<isRequest, Body, Fields> &response) {
    beast::error_code ec;
    http::serializer<isRequest, Body, Fields> sr{response};
    http::write(socket_, sr, ec);
  }

  bool
  handle_request(http::request<http::string_body> request) {
    http::response<http::string_body> response;

    if (resources_.count(std::string(request.target())) == 0) {
      response = {http::status::not_found,
                  request.version()};
      response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
      response.set(http::field::content_type, "text/html");
      response.keep_alive(request.keep_alive());
      response.body() =
          "The resource '" + std::string(request.target()) + "' was not found.";
      response.prepare_payload();
    } else {
      response = resources_[std::string(request.target())].get()->handle_request(
          request);
    }

    send(request, response);
    return request.need_eof();
  }

  tcp::socket& socket_;
  std::map<std::string, std::unique_ptr<http_resource>>& resources_;
};

void http_server::add_resource(std::string path,
                               std::unique_ptr<http_resource> resource) {
  if (running_) {
    return;
  }
  resources_[path] = std::move(resource);
}

void handle(tcp::socket& socket,
            std::map<std::string, std::unique_ptr<http_resource>>* resources) {
  socket_handler(socket, *resources).handle();
}

void http_server::run() {
  if (running_) {
    return;
  }

  running_ = true;

  net::io_context ioc{1};
  tcp::acceptor acceptor{ioc, {net::ip::make_address(address_), port_}};
  for (;;) {
    tcp::socket socket{ioc};
    acceptor.accept(socket);
    std::thread t(std::bind(handle, std::move(socket), &resources_));
    t.detach();
  }
}

}