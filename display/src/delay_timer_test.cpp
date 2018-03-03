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

#include <gtest/gtest.h>
#include "delay_timer.h"

constexpr std::chrono::steady_clock::duration resolution =
    std::chrono::duration<long, std::milli>(1);

constexpr std::chrono::steady_clock::duration want_delay =
    std::chrono::duration<long, std::milli>(1000);

TEST(DelayTimer, TestTimerFired) {
  std::chrono::steady_clock::time_point start_time =
      std::chrono::steady_clock::now();
  bool callback_done = false;
  std::chrono::steady_clock::time_point callback_time;
  airball::DelayTimer t(
      resolution,
      want_delay,
      [&]() {
        if (callback_done) {
          FAIL() << "Callback already done";
        }
        callback_time = std::chrono::steady_clock::now();
        callback_done = true;
      });
  t.start();
  std::this_thread::sleep_for(want_delay * 2);
  std::chrono::steady_clock::duration got_delay = (callback_time - start_time);
  std::chrono::steady_clock::duration delay_error = got_delay - want_delay;
  double delay_error_ratio =
      std::abs(((double) delay_error.count()) / ((double) want_delay.count()));
  ASSERT_LE(delay_error_ratio, 0.01);
}

TEST(DelayTimer, TestTimerCanceled) {
  bool called = false;
  std::chrono::steady_clock::duration want_delay =
      std::chrono::duration<long, std::milli>(500);
  airball::DelayTimer t(
      resolution,
      want_delay,
      [&]() {
        called = true;
      });
  std::this_thread::sleep_for(want_delay / 2);
  t.cancel();
  std::this_thread::sleep_for(want_delay * 2);
  ASSERT_FALSE(called);
}
