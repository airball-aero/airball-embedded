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

#include <functional>
#include <string>
#include <memory>

#include "controller.h"
#include "screen.h"
#include "data_source.h"
#include "user_input_source.h"
#include "data_logger.h"

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

static Options<airball::DataSource, 3> DATA_SOURCE_OPTIONS = {
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
        {
            .arg = std::string("--data_replay"),
            .make = [](){
              return airball::DataSource::NewReplayDataSource("./airball.log");
            },
        },
    },
};

static Options<airball::DataLogger, 2> DATA_LOGGER_OPTIONS = {
    .values = {
        {
            .arg = std::string("--logger_fake"),
            .make = [](){
              return airball::DataLogger::NewFakeDataLogger();
            },
        },
        {
            .arg = std::string("--logger_file"),
            .make = [](){
              return airball::DataLogger::NewFileDataLogger("/tmp/airball.log");
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
  auto data_logger = make(DATA_LOGGER_OPTIONS, argc, argv);
  airball::Controller c(
      screen.get(),
      user_input_source.get(),
      data_source.get(),
      data_logger.get());
  c.run();
}