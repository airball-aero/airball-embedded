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

  radio.enter_command_mode();

  radio.send_command("ATNIAIRBALL_PROBE");
  radio.send_command("ATID=5555");
  radio.send_command("ATMY=7777");
  radio.send_command("ATSM=0");
  radio.send_command("ATSP=64");
  radio.send_command("ATDL=8888");
  radio.send_command("ATAP=1");

  radio.exit_command_mode();

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