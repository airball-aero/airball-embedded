#include <thread>
#include "xbee.h"
#include "format.h"
#include "xbee_api_payload.h"

int main(int argc, char** argv) {
  const std::string airball_serial_device_filename = std::string(argv[1]);

  printf("%s Opening serial port at %s...\n",
         airball::format_time(std::chrono::system_clock::now()).c_str(),
         airball_serial_device_filename.c_str());

  airball::xbee radio(airball_serial_device_filename, 9600);

  radio.enterCommandMode();

  radio.sendCommand("ATNIAIRBALL_PROBE");
  radio.sendCommand("ATID=5555");
  radio.sendCommand("ATMY=7777");
  radio.sendCommand("ATSM=0");
  radio.sendCommand("ATSP=64");
  radio.sendCommand("ATDL=8888");
  radio.sendCommand("ATAP=1");

  radio.exitCommandMode();

  while (true) {
    airball::x01_send_16_bit d(
        0x01,
        0xFFFF,
        0x00,
        "$A,894,116344.30,19.56,2.06,-3.40,1.71");
    radio.write_api_frame(d.frame());
    std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(20));
  }
}