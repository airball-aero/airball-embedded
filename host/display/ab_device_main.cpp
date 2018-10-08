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

#include "controller.h"
#include "../telemetry/xbee_telemetry_client.h"

/**
 * For a description of arguments, see ./raspi-setup.md.
 */
int main(int argc, char **argv) {
  const airball::xbee_known_types::xbee_type type =
      (airball::xbee_known_types::xbee_type)
          std::stoi(argv[1], nullptr, 10);
  auto device = std::string(argv[2]);
  auto gpio_push = std::stoi(argv[3], nullptr, 10);
  auto gpio_encoder_a = std::stoi(argv[4], nullptr, 10);
  auto gpio_encoder_b = std::stoi(argv[5], nullptr, 10);
  airball::Controller c(airball::Screen::NewFramebufferScreen(),
                        airball::UserInputSource::NewGpioInputSource(
                            gpio_push,
                            gpio_encoder_a,
                            gpio_encoder_b),
                        new airball::XbeeTelemetryClient(type, device));
  c.run();
}
