#ifndef AIRBALL_CONTROLLER_H
#define AIRBALL_CONTROLLER_H

#include <string>

#include "screen.h"
#include "user_input_source.h"
#include "data_source.h"

namespace airball {

/**
 * A Controller manages the coordination between the various portions of the
 * Airball application, and contains all the platform independent parts of the
 * logic.
 */
class Controller {
public:
  /**
   * Create a new Controller.
   *
   * @param screen a target onto which to display the output.
   * @param input a source of user interaction events.
   * @param data a source of sensor data.
   */
  Controller(Screen* screen,
             UserInputSource* input,
             DataSource* data);

  /**
   * Run the Controller. Generally, this function never returns.
   */
   void run();

private:
  Screen* screen_;
  UserInputSource* input_;
  DataSource* data_;
};

}  // namespace airball

#endif //AIRBALL_CONTROLLER_H
