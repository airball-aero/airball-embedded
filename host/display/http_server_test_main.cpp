#include "http_server.h"

#include <memory>

namespace http = boost::beast::http;

class test_resource : public airball::http_resource {
public:
  virtual http::response<http::string_body>
  get(http::request<http::string_body> request) {
    http::response<http::string_body> response;
    response.body() = "<h1>Hello world</h1>";
    response.set(http::field::content_type, "text/html");
    return response;
  }
};

int main(int argc, char **argv) {
  airball::http_server s("127.0.0.1", 9090);
  s.add_resource("/test", std::unique_ptr<test_resource>(new test_resource()));
  std::thread t([&]() {
    s.run();
  });
  t.join();
}
