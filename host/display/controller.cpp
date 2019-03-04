/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018, Ihab A.B. Awad
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "controller.h"
#include "settings.h"
#include "airdata.h"
#include "display.h"
#include "delay_timer.h"
#include "system_status.h"
#include "units.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>

namespace airball {

constexpr static std::chrono::duration<unsigned int, std::milli>
    kAdjustKeyStartResolution(10);

constexpr static std::chrono::duration<unsigned int, std::milli>
    kAdjustKeyStartDelay(2000);

constexpr static std::chrono::duration<unsigned int, std::milli>
    kPaintDelay(33);

constexpr static const char* kSettingsPath = "./airball-settings.json";

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

enum Command {
  ADJUST_MODE_ON,
  ADJUST_MODE_OFF,
  ADJUST_UP,
  ADJUST_DOWN,
  ADJUST_NEXT,
  ADJUST_BARO_SETTING_UP,
  ADJUST_BARO_SETTING_DOWN,
};

void apply_command(Settings &s, Command c) {
  switch (c) {
    case ADJUST_MODE_ON:
      s.set_adjusting(true);
      break;
    case ADJUST_MODE_OFF:
      s.set_adjusting(false);
      break;
    case ADJUST_UP:
      s.adjust_up();
      break;
    case ADJUST_DOWN:
      s.adjust_down();
      break;
    case ADJUST_NEXT:
      s.adjust_next();
      break;
    case ADJUST_BARO_SETTING_UP:
      s.adjust_baro_setting_up();
      break;
    case ADJUST_BARO_SETTING_DOWN:
      s.adjust_baro_setting_down();
      break;
    default:
      break;
  }
}

template <typename T>
void foreach(std::vector<T> v, const std::function<void(const T&)>& f) {
 for (auto it = v.begin(); it != v.end(); ++it) {
    f(*it);
  }
}

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
                       UserInputSource* input,
                       TelemetryClient* telemetry)
    : screen_(screen), input_(input), telemetry_(telemetry) {}

void Controller::run() {
  InputQueue<std::unique_ptr<sample>> data;
  InputQueue<std::string> log;
  InputQueue<Command> commands;

  std::mutex input_mutex;
  bool running = true;
  bool adjusting = false;
  bool suppress_adjust_next = false;

  DelayTimer timer(
      kAdjustKeyStartResolution,
      kAdjustKeyStartDelay,
      [&]() {
        std::lock_guard<std::mutex> lock(input_mutex);
        commands.put(adjusting
                     ? Command::ADJUST_MODE_OFF
                     : Command::ADJUST_MODE_ON);
        adjusting = !adjusting;
        suppress_adjust_next = adjusting;
      });

  std::thread input_thread([&]() {
    while (true) {
      auto input = input_->next_input();
      std::lock_guard<std::mutex> lock(input_mutex);
      switch (input) {
        case UserInputSource::Input::ADJUST_KEY_PRESSED:
          timer.start();
          break;
        case UserInputSource::Input::ADJUST_KEY_RELEASED:
          timer.cancel();
          if (adjusting) {
            if (suppress_adjust_next) {
              suppress_adjust_next = false;
            } else {
              commands.put(Command::ADJUST_NEXT);
            }
          }
          break;
        case UserInputSource::Input::ADJUST_UP:
          if (adjusting) {
            commands.put(Command::ADJUST_UP);
          } else {
            commands.put(Command::ADJUST_BARO_SETTING_UP);
          }
          break;
        case UserInputSource::Input::ADJUST_DOWN:
          if (adjusting) {
            commands.put(Command::ADJUST_DOWN);
          } else {
            commands.put(Command::ADJUST_BARO_SETTING_DOWN);
          }
          break;
        case UserInputSource::Input::EXIT:
          running = false;
          break;
        default:
          break;
      }

      if (!running) { break; }
    }
  });

  std::thread data_thread([&]() {
    while (true) {
      data.put(std::move(telemetry_->get()));
      std::lock_guard<std::mutex> lock(input_mutex);
      if (!running) { break; }
    }
  });

  std::thread paint_thread([&]() {
    Settings settings(kSettingsPath);
    SystemStatus status;
    Airdata airdata;
    Display display(screen_, &airdata, &settings, &status);

    while (true) {
      foreach<Command>(commands.get(), [&](const Command& c) {
        apply_command(settings, c);
      });

      // Cannot use foreach for a vector of std::unique_ptr since it invokes
      // copy constructors of the element type which std::unique_ptr deletes.
      std::vector<std::unique_ptr<sample>> cycle_data = data.get();
      for (auto it = cycle_data.begin(); it < cycle_data.end(); ++it) {
        const sample* d = (*it).get();
        auto ad = dynamic_cast<const airdata_sample*>(d);
        if (ad != nullptr) {
          airdata.update(ad, kPascalsPerInHg * settings.baro_setting());
        }
        status.update(d);
      }

      display.paint();

      // TODO(ihab): Build an interrupt triggered paint loop rather than merely
      // sleeping for a fixed delay.
      std::this_thread::sleep_for(kPaintDelay);

      std::lock_guard<std::mutex> lock(input_mutex);
      if (!running) { break; }
    }
  });

  input_thread.join();
  data_thread.join();
  paint_thread.join();
}

}  // namespace airball
