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

#include "delay_timer.h"

#include <iostream>

namespace airball {

DelayTimer::DelayTimer(
    std::chrono::steady_clock::duration resolution,
    std::chrono::steady_clock::duration interval,
    std::function<void()> callback)
    : callback_(callback),
      resolution_(resolution),
      interval_(interval),
      running_(false),
      finished_(false),
      thread_(&DelayTimer::run, this) {}

DelayTimer::~DelayTimer() {
  {
    std::lock_guard<std::mutex> g(mu_);
    finished_ = true;
  }
  thread_.join();
}

void DelayTimer::start() {
  std::lock_guard<std::mutex> g(mu_);
  running_ = true;
  next_wakeup_ = std::chrono::steady_clock::now() + interval_;
}

void DelayTimer::cancel() {
  std::lock_guard<std::mutex> g(mu_);
  running_ = false;
}

bool DelayTimer::running() {
  std::lock_guard<std::mutex> g(mu_);
  return running_;
}

void DelayTimer::run() {
  for (int i = 0; ; i++) {
    std::this_thread::sleep_for(resolution_);
    bool do_callback = false;
    {
      std::lock_guard<std::mutex> g(mu_);
      if (finished_) {
        return;
      }
      if (running_ && std::chrono::steady_clock::now() > next_wakeup_) {
        do_callback = true;
        running_ = false;
      }
    }
    if (do_callback) {
      callback_();
    }
  }
}

}  // namespace airball
