#include <iostream>
#include <vector>
#include <stdlib.h>

#include "airdata.h"

std::vector<std::string> split (std::string s, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr
        << "Usage: " << argv[0] << " qnh" << std::endl
        << "  where qnh = barometric pressure in pascals" << std::endl;
    return -1;
  }
  airball::Airdata airdata;
  std::string line;
  double qnh = atof(argv[1]);
  while (true) {
    getline(std::cin, line);
    if (std::cin.eof()) {
      break;
    }
    auto tokens = split(line, ",");
    if (tokens.size() < 8) {
      continue;
    }
    auto i = tokens.begin();
    double seconds = atof((i++)->c_str());
    uint8_t rssi = atoi((i++)->c_str());
    auto sample_tokens = std::vector<std::string>();
    sample_tokens.emplace_back("$A");
    for (; i != tokens.end(); ++i) {
      sample_tokens.push_back(*i);
    }
    auto millis = std::chrono::milliseconds((int)(seconds / 1000));
    std::chrono::time_point<std::chrono::system_clock> time(millis);
    auto sample = dynamic_cast<airdata_sample*>(
        airdata_sample::create(time, rssi, sample_tokens));
    airdata.update(sample, qnh);

    std::cout <<
              std::fixed << seconds << "," <<
              (int) sample->get_rssi() << "," <<
              sample->get_seq() << "," <<
              sample->get_baro() << "," <<
              sample->get_temperature() << "," <<
              sample->get_dp0() << "," <<
              sample->get_dpA() << "," <<
              sample->get_dpB() << "," <<
              airdata.altitude() << "," <<
              airdata.climb_rate() << "," <<
              airdata.free_stream_q() << "," <<
              airdata.ias() << "," <<
              airdata.tas() << "," <<
              airdata.alpha() << "," <<
              airdata.beta() << std::endl;
  }
}