#include <iostream>
#include <memory>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include "settings.h"
#include "system_status.h"
#include "airdata.h"
#include "display.h"
#include "image_screen.h"
#include "log_reader_telemetry_client.h"
#include "units.h"

constexpr static const char* kOptionHelp =
    "help";
constexpr static const char* kOptionSettingsFilename =
    "settings_filename";
constexpr static const char* kOptionTelemetryLogFilename =
    "telemetry_log_filename";
constexpr static const char* kOptionOutputBase =
    "output_base";

constexpr uint kImageNameBufSize = 2048;

constexpr uint kScreenWidth = 480;
constexpr uint kScreenHeight = 272;

constexpr uint kSamplesPerSecond = 20;
constexpr uint kFramesPerSecond = 25;

constexpr static std::chrono::duration<unsigned int, std::milli>
    kFrameInterval(1000 / kFramesPerSecond);

namespace po = boost::program_options;

static po::options_description add_options() {
  po::options_description desc("Airball replay command line options");
  desc.add_options()
      (
          kOptionHelp,
          "produce help message"
      )
      (
          kOptionSettingsFilename,
          po::value<std::string>(),
          "path to settings JSON file"
      )
      (
          kOptionTelemetryLogFilename,
          po::value<std::string>(),
          "path to telemetry log"
      )
      (
          kOptionOutputBase,
          po::value<std::string>(),
          "base name of output image files"
      );
  return desc;
}

int main(int argc, char **argv) {
  po::options_description options = add_options();
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, options), vm);
  po::notify(vm);

  if (vm.count(kOptionHelp)) {
    std::cout << options << std::endl;
    return 1;
  }

  airball::Settings settings(
      vm[kOptionSettingsFilename].as<std::string>());
  airball::SystemStatus status;
  airball::Airdata airdata;
  auto screen = std::unique_ptr<airball::ImageScreen>(
      new airball::ImageScreen(kScreenWidth, kScreenHeight));
  auto telemetry = std::unique_ptr<airball::TelemetryClient>(
      new airball::LogReaderTelemetryClient(
          vm[kOptionTelemetryLogFilename].as<std::string>(),
          kSamplesPerSecond,
          0,
          false));
  airball::Display display(screen.get(), &airdata, &settings, &status);

  uint64_t img_index = 0;
  char img_name_buf[kImageNameBufSize];

  auto update = [&](const airdata_sample* s) {
    airdata.update(s,
                   kPascalsPerInHg * settings.baro_setting(),
                     settings.ball_smoothing_factor(),
                     settings.vsi_smoothing_factor());
    status.update(static_cast<const class sample*>(s));
  };

  auto paint = [&]() {
    display.paint();
    sprintf(
        img_name_buf,
        "./%s_%09lu.png",
        vm[kOptionOutputBase].as<std::string>().c_str(),
        img_index++);
    screen->write_to_png(img_name_buf);
  };

  bool time_initialized(false);
  std::chrono::system_clock::time_point next_frame_time;

  while (true) {
    auto sample = telemetry->get();
    if (sample == nullptr) { break; }

    auto ad_sample = dynamic_cast<const airdata_sample*>(sample.get());
    if (ad_sample == nullptr) { continue; }

    if (!time_initialized) {
      next_frame_time = ad_sample->get_time();
      time_initialized = true;
    }

    while (next_frame_time < ad_sample->get_time()) {
      paint();
      next_frame_time += kFrameInterval;
    }

    update(ad_sample);
  }
}