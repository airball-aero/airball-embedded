#include <iostream>
#include "udp_packet_reader.h"

int main(int argc, char** argv) {
  airball::udp_packet_reader r(30123);
  if (!r.open()) {
    std::cerr << "Error opening UDP packet reader" << std::endl;
  }
  while (true) {
    std::cout << r.read_packet() << std::endl << std::flush;
  }
}
