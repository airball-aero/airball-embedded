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

#include "one_shot_timer.h"

#include <iostream>

namespace airball {

OneShotTimer::OneShotTimer(
    std::chrono::steady_clock::duration interval,
    std::function<void()> callback)
    : callback_(callback),
      thread_(&OneShotTimer::run, this),
      interval_(interval),
      running_(true) {
  thread_.detach();
}

OneShotTimer::~OneShotTimer() {
  cancel();
}

void OneShotTimer::cancel() {
  mu_.lock();
  running_ = false;
  mu_.unlock();
}

void OneShotTimer::run() {
  std::this_thread::sleep_for(interval_);
  bool canceled;
  mu_.lock();
  canceled = !running_;
  mu_.unlock();
  if (!canceled) { callback_(); }
}

}  // namespace airball
