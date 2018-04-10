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

#ifndef AIRBALL_CONTROLLER_H
#define AIRBALL_CONTROLLER_H

#include <string>

#include "screen.h"
#include "user_input_source.h"
#include "data_logger.h"
#include "../airball_probe_telemetry/telemetry_client.h"

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
   * @param logger a place to log sensor (and other) data.
   */
  Controller(Screen* screen,
             UserInputSource* input,
             TelemetryClient* telemetry,
             DataLogger* logger);

  /**
   * Run the Controller. Generally, this function never returns.
   */
   void run();

private:
  Screen* screen_;
  UserInputSource* input_;
  TelemetryClient* telemetry_;
  DataLogger* logger_;
};

}  // namespace airball

#endif //AIRBALL_CONTROLLER_H
