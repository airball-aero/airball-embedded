#include "controller.h"
#include "settings.h"
#include "airdata.h"
#include "display.h"
#include "delay_timer.h"
#include "system_status.h"
#include "units.h"
#include "../telemetry/airdata_reduced_sample.h"
#include "file_write_watch.h"
#include "sound_scheme.h"
#include "stallfence_scheme.h"
#include "flyonspeed_scheme.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>

namespace airball {

constexpr static std::chrono::duration<unsigned int, std::milli>
    kPaintDelay(33);

const static std::string kStallfenceScheme = "stallfence";
const static std::string kFlyonspeedScheme = "flyonspeed";

template <class T>
class InputQueue {
public:
  void put(T t) {
    std::lock_guard<std::mutex> guard(mu_);
    entries_.push_back(std::move(t));
  }
  std::vector<T> get() {
    std::lock_guard<std::mutex> guard(mu_);
    std::vector<T> result;
    for (auto it = entries_.begin(); it < entries_.end(); ++it) {
      result.push_back(std::move(*it));
    }
    entries_.clear();
    return result;
  }
private:
  std::mutex mu_;
  std::vector<T> entries_;
};

std::chrono::duration<double, std::milli> since(
    const std::chrono::steady_clock::time_point t) {
  return std::chrono::steady_clock::now() - t;
}

std::ostream& operator<<(
    std::ostream &out,
    const std::chrono::duration<double, std::milli> &duration) {
  out << duration.count();
}

Controller::Controller(Screen* screen,
                       const std::string& settings_path,
                       const std::string& audio_device,
                       TelemetryClient* telemetry)
    : screen_(screen),
      settings_path_(settings_path),
      audio_device_(audio_device),
      telemetry_(telemetry) {}

void Controller::run() {
  InputQueue<std::unique_ptr<sample>> data;
  InputQueue<bool> settings_read;
  InputQueue<std::string> log;

  bool running = true;

  std::thread data_thread([&]() {
    while (true) {
      data.put(std::move(telemetry_->get()));
      if (!running) { break; }
    }
  });

  std::thread settings_thread([&]() {
    airball::file_write_watch w(settings_path_);
    while (true) {
      w.next_event();
      settings_read.put(true);
      if (!running) { break; }
    }
  });

  std::thread paint_thread([&]() {
    Settings settings;
    SystemStatus status;
    Airdata airdata;
    Display display(screen_, &airdata, &settings, &status);

    settings.load(settings_path_);

    std::unique_ptr<sound_scheme> sound_scheme;
    if (settings.sound_scheme() == kStallfenceScheme) {
      sound_scheme.reset(new stallfence_scheme(audio_device_,
                                               &settings,
                                               &airdata));
    } else if (settings.sound_scheme() == kFlyonspeedScheme) {
      sound_scheme.reset(new flyonspeed_scheme(audio_device_,
                                               &settings,
                                               &airdata));
    } else {
      std::cerr << "Unrecognized sound scheme "
                << settings.sound_scheme() << std::endl;
      std::exit(-1);
    }

    if (!sound_scheme->start()) {
      std::cerr << "Sound scheme failed to start" << std::endl;
      std::exit(-1);
    }

    while (true) {
      std::vector<bool> settings_reads = settings_read.get();
      std::vector<std::unique_ptr<sample>> cycle_data = data.get();

      if (settings_reads.size() > 0) {
        settings.load(settings_path_);
      }

      for (auto it = cycle_data.begin(); it < cycle_data.end(); ++it) {
        const sample* d = (*it).get();
        status.update(d);
        const double qnh = kPascalsPerInHg * settings.baro_setting();
        // TODO(ihab): Deprecate and remove all "raw airdata" operations from
        // the user interface.
        auto ad = dynamic_cast<const airdata_sample*>(d);
        if (ad != nullptr) {
          airdata.update(ad,
                         qnh,
                         settings.ball_smoothing_factor(),
                         settings.vsi_smoothing_factor());
        } else {
          auto adr = dynamic_cast<const airdata_reduced_sample *>(d);
          if (adr != nullptr) {
            airdata.update(adr,
                           qnh,
                           settings.ball_smoothing_factor(),
                           settings.vsi_smoothing_factor());
          }
        }
      }

      sound_scheme->update();
      display.paint();

      // TODO(ihab): Build an interrupt triggered paint loop rather than merely
      // sleeping for a fixed delay.
      std::this_thread::sleep_for(kPaintDelay);

      if (!running) { break; }
    }
  });

  data_thread.join();
  settings_thread.join();
  paint_thread.join();
}

}  // namespace airball
