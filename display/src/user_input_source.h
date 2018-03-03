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

#ifndef AIRBALL_USER_INPUT_SOURCE_H
#define AIRBALL_USER_INPUT_SOURCE_H

#include <string>

namespace airball {

/**
 * Encapsulates a source of user input, e.g. from pushbuttons.
 */
class UserInputSource {
public:

  virtual ~UserInputSource() {};

  /**
   * The type of input selected by the user.
   */
  enum Input : unsigned int {
    /**
     * Placeholder to indicate no input.
     */
    NONE = 0,

    /**
     * Indicates the "Adjust" key has been pressed down. A time delay is used
     * to actually enter adjust mode.
     */
    ADJUST_KEY_PRESSED = 1,

    /**
     * Indicates the "Adjust" key has been released.
     */
    ADJUST_KEY_RELEASED = 2,

    /**
     * Indicates the key to increase the current adjustable parameter (if any)
     * has been pressed.
     */
    ADJUST_UP = 3,

    /**
     * Indicates the key to decrease the current adjustable parameter (if any)
     * has been pressed.
     */
    ADJUST_DOWN = 4,

    /**
     * Indicates application exit. Used only for development.
     */
     EXIT = 5,
  };

  /**
   * Blocks until the next available user input is available, then returns
   * it.
   */
  virtual Input next_input() = 0;

  /**
   * Creates a UserInputSource that can be run in a Linux terminal, and which
   * takes input from the keyboard.
   *
   * TODO: Document key mappings.
   *
   * @return a UserInputSource.
   */
  static UserInputSource* NewKeyboardInputSource();

  /**
   * Creates a UserInputSource that takes inputs from GPIO pins connected to
   * actual input hardware.
   *
   * TODO: Document pin mappings
   *
   * @return a UserInputSource.
   */
  static UserInputSource* NewGpioInputSource(int gpio_push,
                                             int gpio_encoder_a,
                                             int gpio_encoder_b);

  /**
   * Creates a UserInputSource that uses X11 events, for environments where X11
   * is available.
   *
   * TODO: Document key mappings.
   *
   * @return a UserInputSource.
   */
   static UserInputSource* NewX11InputSource();
};

}  // namespace airball

#endif //AIRBALL_USER_INPUT_SOURCE_H
