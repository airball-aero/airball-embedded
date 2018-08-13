#include <thread>
#include "xbee.h"
#include "format.h"

int main(int argc, char** argv) {
  const std::string airball_serial_device_filename = std::string(argv[1]);

  printf("%s Opening serial port at %s...\n",
         airball::format_time(std::chrono::system_clock::now()).c_str(),
         airball_serial_device_filename.c_str());

  airball::xbee radio(airball_serial_device_filename, 9600, airball::xbee::DATA_SOURCE);
  radio.initialize();

  while (true) {
    radio.send("$A,894,116344.30,19.56,2.06,-3.40,1.71");
    std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(20));
  }
}