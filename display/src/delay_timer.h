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

#ifndef AIRBALL_EXCLUSIVE_DELAY_TIMER_H
#define AIRBALL_EXCLUSIVE_DELAY_TIMER_H

#include <chrono>
#include <functional>
#include <thread>
#include <mutex>

namespace airball {

/**
 * A DelayTimer keeps track of a given delay time, and when that time
 * has elapsed, it fires an action. It can be canceled and restarted as many
 * times as needed.
 */
class DelayTimer {
public:
  /**
   * Create a new DelayTimer.
   *
   * @param resolution the resolution of the timer. The timer will not be able
   *     to maintain time intervals with any finer precision than this value.
   *     However, the finer the value, the larger the constant CPU time the
   *     timer will consume.
   * @param interval the interval that the timer will wait before calling the
   *     supplied function.
   * @param callback the function that will be called when the timer fires.
   */
  DelayTimer(
      std::chrono::steady_clock::duration resolution,
      std::chrono::steady_clock::duration interval,
      std::function<void()> callback);
  ~DelayTimer();

  /**
   * Start the timer. If the timer is already running but has not fired, this
   * resets the timer based on the current time and keeps it running.
   */
  void start();

  /**
   * Cancel the timer. If the timer is not running, this is a no-op.
   */
  void cancel();

  /**
   * Returns whether the timer is running.
   */
  bool running();

private:
  void run();

  const std::function<void()> callback_;
  std::thread thread_;
  std::mutex mu_;
  std::chrono::steady_clock::duration resolution_;
  std::chrono::steady_clock::duration interval_;
  std::chrono::steady_clock::time_point next_wakeup_;
  bool running_;
  bool finished_;
};

}  // namespace airball

#endif //AIRBALL_EXCLUSIVE_DELAY_TIMER_H
