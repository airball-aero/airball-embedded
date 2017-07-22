#include <gtest/gtest.h>
#include "one_shot_timer.h"

constexpr std::chrono::steady_clock::duration want_delay =
    std::chrono::duration<long, std::milli>(500);

TEST(OneShotTimer, TestTimerFired) {
  std::chrono::steady_clock::time_point start_time =
      std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point callback_time;
  airball::OneShotTimer t(
      want_delay,
      [&]() {
        callback_time = std::chrono::steady_clock::now();
      });
  std::this_thread::sleep_for(want_delay * 2);
  std::chrono::steady_clock::duration delay_error =
      (callback_time - start_time) - want_delay;
  double delay_error_ratio =
      ((double) delay_error.count()) / ((double) want_delay.count());
  ASSERT_LE(delay_error_ratio, 0.01);
}

TEST(OneShotTimer, TestTimerCanceled) {
  bool called = false;
  airball::OneShotTimer t(
      want_delay,
      [&]() {
        called = true;
      });
  std::this_thread::sleep_for(want_delay / 2);
  t.cancel();
  std::this_thread::sleep_for(want_delay * 2);
  ASSERT_FALSE(called);
}
