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

#include "user_input_source.h"

#include <ratio>
#include <thread>
#include <iostream>
#include <termios.h>
#include <zconf.h>
#include <fstream>
#include <fcntl.h>
#include <poll.h>
#include <backward/strstream>
#include <sstream>
#include <cstring>

namespace airball {

class KeyboardUserInputSource : public UserInputSource {
public:
  KeyboardUserInputSource();
  virtual ~KeyboardUserInputSource();
  virtual Input next_input() override;
private:
  struct termios old_settings_;
};

KeyboardUserInputSource::KeyboardUserInputSource() {
  tcgetattr(STDIN_FILENO, &old_settings_);
  struct termios new_settings = old_settings_;
  new_settings.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
  std::cout
      << "KeyboardUserInputSource usage:" << std::endl
      << "  a: ADJUST key pressed" << std::endl
      << "  r: ADJUST key released" << std::endl
      << "  u: ADJUST up" << std::endl
      << "  d: ADJUST down" << std::endl
      << "  x: exit" << std::endl;
}

KeyboardUserInputSource::~KeyboardUserInputSource() {
  tcsetattr(STDIN_FILENO, TCSANOW, &old_settings_);
}

UserInputSource::Input KeyboardUserInputSource::next_input() {
  char d;
  while (read(STDIN_FILENO, &d, 1)) {
    switch (d) {
      case 'a':
      case 'A':
        return Input::ADJUST_KEY_PRESSED;
      case 'r':
      case 'R':
        return Input::ADJUST_KEY_RELEASED;
      case 'u':
      case 'U':
        return Input::ADJUST_UP;
      case 'd':
      case 'D':
        return Input::ADJUST_DOWN;
      case 'x':
      case 'X':
        return Input::EXIT;
      default:
        break;
      }
  }
}

UserInputSource* UserInputSource::NewKeyboardInputSource() {
  return new KeyboardUserInputSource();
}

constexpr int kGpioIndexPush = 0;
constexpr int kGpioIndexEncoderA = 1;
constexpr int kGpioIndexEncoderB = 2;
constexpr int kNumGpio = 3;

class GpioUserInputSource : public UserInputSource {
public:
  GpioUserInputSource(int gpio_push,
                      int gpio_encoder_a,
                      int gpio_encoder_b);
  virtual ~GpioUserInputSource();
  virtual Input next_input() override;
private:
  struct pollfd fds_[kNumGpio];
  Input pending_input_;
  int last_value_push_;
  int last_value_encoder_a_;
  int last_value_encoder_b_;
};

std::string str(const int k) {
  std::ostringstream s;
  s << k;
  return s.str();
}

std::string gpio_dir(const int k) {
  return "/sys/class/gpio/gpio" + str(k);
}

void echo(std::string name, std::string value) {
  std::fstream fs;
  fs.open(name, std::fstream::out);
  fs << value << std::endl;
  fs.close();
}

void export_gpio(const int k) {
  echo("/sys/class/gpio/export", str(k));
}

void wait_for_export(const int k) {
  while (true) {
    std::fstream fs;
    fs.open(gpio_dir(k) + "/value", std::fstream::in);
    if (fs.is_open()) {
      return;
    }
  }
}

int setup_and_open(const int k) {
  echo(gpio_dir(k) + "/direction", "in");
  echo(gpio_dir(k) + "/edge", "both");
  int fd = open((gpio_dir(k) + "/value").c_str(), O_RDONLY | O_NONBLOCK);
  if (fd < 0) {
    perror("Unable to open specified device");
  }
  return fd;
}

void setup(struct pollfd fds[], int index, int gpio) {
  export_gpio(gpio);
  wait_for_export(gpio);
  fds[index].fd = setup_and_open(gpio);
  fds[index].events = POLLPRI;
}

int read_fd(int fd) {
  char c[3];
  c[2] = '\0';
  lseek(fd, 0, SEEK_SET);
  ssize_t n = read(fd, c, 2);
  return atoi(c);
}

GpioUserInputSource::GpioUserInputSource(int gpio_push,
                                         int gpio_encoder_a,
                                         int gpio_encoder_b)
    : pending_input_(Input::NONE),
      last_value_push_(0),
      last_value_encoder_a_(0),
      last_value_encoder_b_(0) {
  memset(fds_, sizeof(fds_), 0);
  setup(fds_, kGpioIndexPush, gpio_push);
  setup(fds_, kGpioIndexEncoderA, gpio_encoder_a),
  setup(fds_, kGpioIndexEncoderB, gpio_encoder_b);
}

GpioUserInputSource::~GpioUserInputSource() {}

UserInputSource::Input GpioUserInputSource::next_input() {
  Input result = Input::NONE;

  // Early return in case we had two events happen at the same time and
  // we're saving on of them up to return it the next time we are called
  if (pending_input_ != Input::NONE) {
    result = pending_input_;
    pending_input_ = Input::NONE;
    return result;
  }

  Input input_push = Input::NONE;
  Input input_encoder = Input::NONE;

  while (true) {

    int r = poll(fds_, 3, -1);

     if (r <= 0) {
       perror("poll");
       continue;
     }

    // Unconditionally re-evaluate all the inputs; it's easier this way
    int current_value_push = read_fd(fds_[kGpioIndexPush].fd);
    int current_value_encoder_a = read_fd(fds_[kGpioIndexEncoderA].fd);
    int current_value_encoder_b = read_fd(fds_[kGpioIndexEncoderB].fd);

    if (current_value_push != last_value_push_) {
      // A pushbutton event occurred
      input_push = (current_value_push == 1)
          ? Input::ADJUST_KEY_PRESSED : Input::ADJUST_KEY_RELEASED;
     }

    if (current_value_encoder_a != last_value_encoder_a_) {
      // Detected an edge on encoder signal A
      if (current_value_encoder_a == 0 && current_value_encoder_b == 1) {
        // Moved in the direction of A
        input_encoder = Input::ADJUST_UP;
      }
    } else if (current_value_encoder_b != last_value_encoder_b_) {
      // Detected an edge on encoder signal B
      if (current_value_encoder_b == 0 && current_value_encoder_a == 1) {
        // Moved in the direction of B
        input_encoder = Input::ADJUST_DOWN;
      }
    }

    // Stash away the current values for the next loop
    last_value_push_ = current_value_push;
    last_value_encoder_a_ = current_value_encoder_a;
    last_value_encoder_b_ = current_value_encoder_b;

    // Decide whether to store an input
    if (input_push != Input::NONE && input_encoder != Input::NONE) {
      // We need to remember something for posterity
      pending_input_ = input_push;
      result = input_encoder;
    } else if (input_push != Input::NONE) {
      pending_input_ = Input::NONE;
      result = input_push;
    } else if (input_encoder != Input::NONE) {
      pending_input_ = Input::NONE;
      result = input_encoder;
    }

    break;
  }

  // std::cout << "result = " << result << std::endl;

  return result;
}

UserInputSource* UserInputSource::NewGpioInputSource(int gpio_push,
                                                     int gpio_encoder_a,
                                                     int gpio_encoder_b) {
  return new GpioUserInputSource(gpio_push,
                                 gpio_encoder_a,
                                 gpio_encoder_b);
}

}  // namespace airball
