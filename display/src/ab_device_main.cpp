#include "controller.h"
#include "screen.h"
#include "data_source.h"
#include "data_logger.h"
#include "user_input_source.h"

int main(int argc, char **argv) {
  airball::Controller c(airball::Screen::NewFramebufferScreen(),
                        airball::UserInputSource::NewGpioInputSource(2, 3, 4),
                        airball::DataSource::NewSerialDataSource("/dev/ttyAMA0"),
                        airball::DataLogger::NewFileDataLogger("/airball.log"));
  c.run();
}
