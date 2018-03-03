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

#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

namespace airball {

/**
 * A OneShotTimer allows a client to arrange for a given function to be called
 * after a given period of wall clock time has elapsed.
 */
class OneShotTimer {
public:
  /**
   * Create a new Timer.
   *
   * @param interval the interval that the timer will wait before calling the
   *     supplied function.
   * @param callback the function that will be called when the timer fires.
   */
  OneShotTimer(
      std::chrono::steady_clock::duration interval,
      std::function<void()> callback);
  ~OneShotTimer();

  /**
   * Cancel the timer. The timer cannot be re-used.
   */
  void cancel();

private:
  void run();

  const std::function<void()> callback_;
  std::thread thread_;
  std::mutex mu_;
  std::chrono::steady_clock::duration interval_;
  bool running_;
};

}  // namespace airball