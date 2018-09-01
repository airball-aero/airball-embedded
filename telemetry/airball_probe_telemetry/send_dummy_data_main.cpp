#include <thread>
#include "xbee.h"
#include "format.h"
#include "xbee_api_payload.h"
#include "xbee_known_types.h"

static const char* const DUMMY_DATA =
    "$A,894,116344.30,19.56,2.06,-3.40,1.71";

int main(int argc, char** argv) {
  // TODO: Not a good idea to directly pass enum values as an integer.
  const airball::xbee_known_types::xbee_type type =
      (airball::xbee_known_types::xbee_type)
          std::stoi(argv[1], nullptr, 10);

  const std::string airball_serial_device_filename = std::string(argv[2]);

  printf("%s Opening serial port at %s...\n",
         airball::format_time(std::chrono::system_clock::now()).c_str(),
         airball_serial_device_filename.c_str());

  airball::xbee radio(airball_serial_device_filename, 9600);

  radio.enter_api_mode();

  switch (type) {
    case airball::xbee_known_types::xbee_802_14:
      radio.write_api_frame(
          airball::x08_at_command(0x02, "NI", "AIRBALL_PROBE").frame());
      radio.write_api_frame(
          airball::x08_at_command(0x03, "ID", (uint16_t) 0x5555).frame());
      radio.write_api_frame(
          airball::x08_at_command(0x04, "MY", (uint16_t) 0x7777).frame());
      radio.write_api_frame(
          airball::x08_at_command(0x05, "SM", (uint8_t) 0x00).frame());
      radio.write_api_frame(
          airball::x08_at_command(0x06, "SM", (uint8_t) 0x00).frame());
      radio.write_api_frame(
          airball::x08_at_command(0x07, "DL", (uint16_t) 0x8888).frame());
      break;
    case airball::xbee_known_types::xbee_900mhz:
      // For the 900 MHz radios, we just broadcast
      break;
    default:
      std::cout << "Unknown XBee type " << type << std::endl;
      exit(-1);
  }

  while (true) {
    switch (type) {
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