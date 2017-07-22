#include <iostream>
#include "data_source.h"

int main(int argc, char** argv) {
  auto sds = airball::DataSource::NewSerialDataSource(argv[1]);
  while (true) {
    std::cout << sds->next_data_sentence() << std::endl;
  }
}
