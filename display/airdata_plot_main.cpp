#include <iostream>
#include "data_source.h"
#include "airdata.h"

int main(int argc, char** argv) {
  airball::Airdata airdata;
  airball::DataSource* data_source = airball::DataSource::NewFakeDataSource();
  for (int i = 0; i < 5000; i++) {
    auto s = data_source->next_data_sentence();
    airdata.update_from_sentence(s);
    std::cout
        << s << ","
        << airdata.alpha() << ","
        << airdata.beta() << ","
        << airdata.ias() << ","
        << std::endl;
  }
}
