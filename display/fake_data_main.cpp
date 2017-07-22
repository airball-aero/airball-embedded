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
