#include <functional>
#include <string>
#include <memory>

#include "controller.h"
#include "screen.h"
#include "data_source.h"
#include "user_input_source.h"

template <class T> struct Option {
  std::string arg;
  const std::function<T*()> make;
};

template <class T, int N> struct Options {
  const Option<T> values[N];
};

static Options<airball::Screen, 2> SCREEN_OPTIONS = {
    .values = {
        {
            .arg = std::string("--screen_x11"),
            .make = [](){
              return airball::Screen::NewX11Screen(800, 480);
            },
        },
        {
            .arg = std::string("--screen_fb"),
            .make = [](){
              return airball::Screen::NewFramebufferScreen();
            },
        },
    },
};

static Options<airball::UserInputSource, 2> USER_INPUT_SOURCE_OPTIONS = {
    .values = {
        {
            .arg = std::string("--input_kbd"),
            .make = [](){
              return airball::UserInputSource::NewKeyboardInputSource();
            },
        },
        {
            .arg = std::string("--input_gpio"),
            .make = [](){
              return airball::UserInputSource::NewGpioInputSource(2, 3, 4);
            },
        },
    },
};

static Options<airball::DataSource, 2> DATA_SOURCE_OPTIONS = {
    .values = {
        {
            .arg = std::string("--data_fake"),
            .make = [](){
              return airball::DataSource::NewFakeDataSource();
            },
        },
        {
            .arg = std::string("--data_serial"),
            .make = [](){
              return airball::DataSource::NewSerialDataSource("/dev/ttyS0");
            },
        },
    },
};

template <class T, int N> std::unique_ptr<T> make(
    const Options<T, N> &options,
    int argc,
    char**argv) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < argc; j++) {
      if (options.values[i].arg.compare(argv[j]) == 0) {
        return std::unique_ptr<T>(options.values[i].make());
      }
    }
  }
  return std::unique_ptr<T>(options.values[0].make());
}

int main(int argc, char** argv) {
  auto screen = make(SCREEN_OPTIONS, argc, argv);
  auto user_input_source = make(USER_INPUT_SOURCE_OPTIONS, argc, argv);
  auto data_source = make(DATA_SOURCE_OPTIONS, argc, argv);
  airball::Controller c(
      screen.get(),
      user_input_source.get(),
      data_source.get());
  c.run();
}