#include <iostream>
#include "user_input_source.h"

int main(int argc, char** argv) {
  auto uis = airball::UserInputSource::NewGpioInputSource(2, 3, 4);
  while (true) {
    auto input = uis->next_input();
    if (input != airball::UserInputSource::Input::NONE) {
      std::cout << "-> Got input = " << input << std::endl;
    }
  }
}
