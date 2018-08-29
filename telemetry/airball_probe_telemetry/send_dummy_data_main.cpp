#include <thread>
#include "xbee.h"
#include "format.h"
#include "xbee_api_payload.h"
#include "xbee_known_types.h"

static const char* const DUMMY_DATA =
    "$A,894,116344.30,19.56,2.06,-3.40,1.71";

int main(int argc, char** argv) {
  const std::string airball_serial_device_filename = std::string(argv[1]);

  printf("%s Opening serial port at %s...\n",
         airball::format_time(std::chrono::system_clock::now()).c_str(),
         airball_serial_device_filename.c_str());

  airball::xbee radio(airball_serial_device_filename, 9600);

  radio.enter_command_mode();

  auto xbee_type = airball::xbee_known_types::get_xbee_type(
      radio.get_hardware_version());

  switch (xbee_type) {
    case airball::xbee_known_types::xbee_802_14:
      radio.send_command("ATNIAIRBALL_PROBE");
      radio.send_command("ATID=5555");
      radio.send_command("ATMY=7777");
      radio.send_command("ATSM=0");
      radio.send_command("ATSP=64");
      radio.send_command("ATDL=8888");
      radio.send_command("ATAP=1");
      break;
    case airball::xbee_known_types::xbee_900mhz:
      // For the 900 MHz radios, we just broadcast
      radio.send_command("ATAP 1");
      break;
    default:
      std::cout << "Unknown XBee type " << xbee_type << std::endl;
      exit(-1);
  }

  radio.exit_command_mode();

  while (true) {
    switch (xbee_type) {
      case airball::xbee_known_types::xbee_802_14:
        radio.write_api_frame(
            airball::x01_send_16_bit(
                0x01,
                0xFFFF,
                0x00,
                DUMMY_DATA).frame());
        break;
      case airball::xbee_known_types::xbee_900mhz:
        radio.write_api_frame(
            airball::x10_send_64_bit(
                0x01,
                0x000000000000ffff,
                0xffff,
                0x00,
                0x00,
                DUMMY_DATA).frame());
        break;
    }
    std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(20));
  }
}