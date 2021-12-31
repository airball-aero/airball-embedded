#include <functional>
#include <iostream>
#include <memory>
#include "boost/program_options.hpp"
#include "controller.h"
#include "fake_telemetry_client.h"
#include "log_reader_telemetry_client.h"
#include "../telemetry/xbee_telemetry_client.h"
#include "../telemetry/esp32_telemetry_client.h"

namespace po = boost::program_options;

static po::options_description add_options() {
  po::options_description desc("Airball test command line options");
  desc.add_options()
      (
          "help",
          "produce help message"
      )
      (
          "screen",
          po::value<std::string>(),
          "screen type (x11, fb)"
      )
      (
          "settings_path",
          po::value<std::string>(),
          "path to settings file"
      )
      (
          "audio_device",
          po::value<std::string>(),
          "audio device (e.g. default, hw:0, ...)"
      )
      (
          "telemetry",
          po::value<std::string>(),
          "telemetry source (fake, log, xbee, esp32)"
      )
      (
          "telemetry_log_filename",
          po::value<std::string>(),
          "telemetry=log filename"
      )
      (
          "telemetry_log_samples_per_second",
          po::value<uint>(),
          "telemetry=log samples per second in input data"
      )
      (
          "telemetry_log_start_sample",
          po::value<uint>(),
          "telemetry=log first sample to skip to"
      )
      (
          "telemetry_xbee_device",
          po::value<std::string>(),
          "telemetry=probe serial device"
      );
  return desc;
}

int main(int argc, char **argv) {
  po::options_description options = add_options();
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, options), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << options << std::endl;
    return 1;
  }

  std::unique_ptr<airball::Screen> screen;
  {
    std::string v = vm["screen"].as<std::string>();
    if (v == "x11") {
      screen = std::unique_ptr<airball::Screen>
          (airball::Screen::NewX11Screen(480, 272));
    } else if (v == "fb") {
      screen = std::unique_ptr<airball::Screen>
          (airball::Screen::NewFramebufferScreen());
    } else {
      std::cout << "Invalid screen option " << v << std::endl;
      std::cout << options << std::endl;
      return 1;
    }
  }

  std::string settings_path = vm["settings_path"].as<std::string>();

  std::string audio_device = vm["audio_device"].as<std::string>();

  std::unique_ptr<airball::TelemetryClient> telemetry;
  {
    std::string v = vm["telemetry"].as<std::string>();
    if (v == "fake") {
      telemetry = std::unique_ptr<airball::TelemetryClient>
          (new airball::FakeTelemetryClient());
    } else if (v == "log") {
      if (vm.count("telemetry_log_filename") != 1 ||
          vm.count("telemetry_log_samples_per_second") != 1 ||
          vm.count("telemetry_log_start_sample") != 1) {
        std::cerr << "Option telemetry=log requires telemetry_log_* options"
                  << std::endl;
        std::cerr << options << std::endl;
        return 1;
      }
      telemetry = std::unique_ptr<airball::TelemetryClient>
          (new airball::LogReaderTelemetryClient(
              vm["telemetry_log_filename"].as<std::string>(),
              vm["telemetry_log_samples_per_second"].as<uint>(),
              vm["telemetry_log_start_sample"].as<uint>(),
              true));
    } else if (v == "xbee") {
      if (vm.count("telemetry_xbee_device") != 1) {
        std::cerr << "Option telemetry=probe requires telemetry_xbee_* options"
                  << std::endl;
        std::cerr << options << std::endl;
        return 1;
      }
      telemetry = std::unique_ptr<airball::TelemetryClient>
          (new airball::XbeeTelemetryClient(
              vm["telemetry_xbee_device"].as<std::string>()));
    } else if (v == "esp32") {
      telemetry = std::unique_ptr<airball::TelemetryClient>
          (new airball::ESP32TelemetryClient());
    } else {
      std::cout << "Invalid telemetry option " << v << std::endl;
      std::cout << options << std::endl;
      return 1;
    }
  }

  airball::Controller c(
      screen.get(),
      settings_path,
      audio_device,
      telemetry.get());
  c.run();
}