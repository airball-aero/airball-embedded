#include <iostream>
#include <backward/strstream>
#include <fstream>

#include "format.h"
#include "xbee_telemetry_client.h"
#include "xbee_known_types.h"
#include "airdata_sample.h"

void print_quantity(const long int value, const char symbol) {
  std::ostrstream s;
  for (int i = 0; i < value; i++) {
    if (i % 10 == 0) {
      s << "|";
    } else {
      s << symbol;
    }
  }
  s << std::ends;
  printf("%03ld %s\n", value, s.str());
}

constexpr unsigned int AIRDATA_BUF_LEN = 1024;
char airdata_buf[AIRDATA_BUF_LEN];

int main(int argc, char **argv) {
  // TODO: Not a good idea to directly pass enum values as an integer.
  const airball::xbee_known_types::xbee_type type =
      (airball::xbee_known_types::xbee_type)
          std::stoi(argv[1], nullptr, 10);
  auto device = std::string(argv[2]);

  airball::XbeeTelemetryClient telemetry_client(type, device);

  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"

  auto previous_loop_time = std::chrono::system_clock::now();

  for (int packet_number=0; ; packet_number++) {
    std::unique_ptr<sample> sample = telemetry_client.get();
    auto now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration since_last =
        now.time_since_epoch() -
        previous_loop_time.time_since_epoch();
    auto since_last_mills =
        std::chrono::duration_cast<std::chrono::duration<unsigned int, std::milli>>(since_last);
    previous_loop_time = now;
    print_quantity(static_cast<int>(sample->get_rssi()), '+');
    print_quantity(since_last_mills.count(), '.');
    auto ads = dynamic_cast<airdata_sample*>(sample.get());
    if (ads != nullptr) {
      ads->snprintf(airdata_buf, AIRDATA_BUF_LEN);
      std::cout << airdata_buf << std::endl;
    }
  }

  #pragma clang diagnostic pop

  return 0;
}
