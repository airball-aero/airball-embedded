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
