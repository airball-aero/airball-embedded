#include <iostream>
#include <backward/strstream>
#include <fstream>

#include "format.h"
#include "xbee_telemetry_client.h"
#include "xbee_known_types.h"

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
  }

  #pragma clang diagnostic pop

  return 0;
}