#include "controller.h"
#include "../telemetry/xbee_telemetry_client.h"
#include "device_keyboard_listener.h"

#include <thread>

/**
 * For a description of arguments, see ./raspi-setup.md.
 */
int main(int argc, char **argv) {
  auto device = std::string(argv[1]);
  airball::Controller c(airball::Screen::NewFramebufferScreen(),
                        "/airball-settings.json",
                        new airball::XbeeTelemetryClient(device));
  airball::DeviceKeyboardListener dkl;
  new std::thread([&]() {
    dkl.run();
  });
  c.run();
}
