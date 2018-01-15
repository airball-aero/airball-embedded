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
