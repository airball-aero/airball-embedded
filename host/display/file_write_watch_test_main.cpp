#include <iostream>
#include "file_write_watch.h"

int main(int arg, char** argv) {
  airball::file_write_watch w(argv[1]);
  while (true) {
    w.next_event();
    std::cout << "write to " << argv[1] << std::endl;
  }
}
