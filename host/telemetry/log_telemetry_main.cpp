// This program logs telemetry, with a timestamp, to standard output (which can
// be redirected to a file).

#include <iostream>
#include <backward/strstream>
#include <fstream>

#include "format.h"
#include "xbee_telemetry_client.h"
#include "xbee_known_types.h"
#include "airdata_sample.h"

constexpr unsigned int BUF_LEN = 1024;
char print_buf[BUF_LEN];

#define AIR_DATA     "airdata: "
#define BATTERY_DATA "battery: "

int main(int argc, char **argv) {
  auto device = std::string(argv[1]);

  airball::XbeeTelemetryClient telemetry_client(device);

  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"

  for (;;) {
    const std::unique_ptr<sample> s = telemetry_client.get();
    print_buf[0] = '\0';
    {
      auto st = dynamic_cast<const airdata_sample*>(s.get());
      if (st != nullptr) {
        st->snprintf(print_buf, BUF_LEN);
        std::cout << AIR_DATA << print_buf << std::endl;
      }
    }
    {
      auto st = dynamic_cast<const battery_sample *>(s.get());
      if (st != nullptr) {
        st->snprintf(print_buf, BUF_LEN);
        std::cout << BATTERY_DATA << print_buf << std::endl;
      }
    }
  }

  #pragma clang diagnostic pop

  return 0;
}
