#include "controller.h"
#include "settings.h"
#include "airdata.h"
#include "display.h"
#include "delay_timer.h"

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

template <class T> class InputQueue {
public:
  void put(const T& t) {
    std::lock_guard<std::mutex> guard(mu_);
    entries_.push_back(t);
  }
  std::vector<T> get() {
    std::lock_guard<std::mutex> guard(mu_);
    std::vector<T> result = entries_;
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
    default:
      break;
  }
}

template <typename T>
void foreach(std::vector<T> v, const std::function<void(const T&)>& f) {
  for (typename std::vector<T>::iterator it = v.begin(); it != v.end(); ++it) {
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

Controller::Controller(Screen *screen,
                       UserInputSource *input,
                       DataSource *data)
    : screen_(screen), input_(input), data_(data) {}

void Controller::run() {
  InputQueue<std::string> data;
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
          }
          break;
        case UserInputSource::Input::ADJUST_DOWN:
          if (adjusting) {
            commands.put(Command::ADJUST_DOWN);
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
      data.put(data_->next_data_sentence());
      std::lock_guard<std::mutex> lock(input_mutex);
      if (!running) { break; }
    }
  });

  std::thread paint_thread([&]() {
    Settings settings(kSettingsPath);
    Airdata airdata;
    Display display(screen_, &airdata, &settings);
    airdata.update_from_sentence("");
    while (true) {
      auto time_start = std::chrono::steady_clock::now();
      int num_commands = 0;
      foreach<Command>(commands.get(), [&](const Command& c) {
        num_commands++;
        apply_command(settings, c);
      });
      auto commands_applied = since(time_start);
      int num_data_sentences = 0;
      foreach<std::string>(data.get(), [&](const std::string& s) {
        num_data_sentences++;
        airdata.update_from_sentence(s);
      });
      auto data_sentences_applied = since(time_start);
      display.paint();
      auto painted = since(time_start);
      std::this_thread::sleep_for(kPaintDelay);
      time_start = std::chrono::steady_clock::now();
      std::lock_guard<std::mutex> lock(input_mutex);
      auto locked_input_mutex = since(time_start);
      std::cout
          << num_commands << ","
          << num_data_sentences << ","
          << commands_applied << ","
          << data_sentences_applied << ","
          << painted << ","
          << locked_input_mutex << std::endl;
      if (!running) { break; }
    }
  });

  input_thread.join();
  data_thread.join();
  paint_thread.join();
}

}  // namespace airball
