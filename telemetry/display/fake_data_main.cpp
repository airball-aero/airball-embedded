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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include <thread>
#include <sstream>

#include "data_source.h"

constexpr static std::chrono::duration<unsigned int, std::milli>
    kSendDelay(10);

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <serial-device>" << std::endl;
    return 0;
  }
  int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    perror("Unable to open specified device");
    return -1;
  }
  struct termios options;
  tcgetattr(fd, &options);
  cfsetispeed(&options, B9600);
  cfsetospeed(&options, B9600);
  options.c_cflag |= (CLOCAL | CREAD);
  if (tcsetattr(fd, TCSANOW, &options) < 0) {
    perror("Unable to set attributes on device");
    return -1;
  }
  airball::DataSource *ds = airball::DataSource::NewFakeDataSource();
  while (true) {
    std::string data = ds->next_data_sentence();
    write(fd, data.c_str(), data.length() - 1);
    write(fd, "\n", 1);
    std::cout << data << std::endl;
    std::this_thread::sleep_for(kSendDelay);
  }
}
