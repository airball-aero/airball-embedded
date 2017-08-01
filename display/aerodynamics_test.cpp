#include "aerodynamics.h"

#include "gtest/gtest.h"

namespace airball {

TEST(Airdata, ias_to_q) {
  EXPECT_EQ(6000, ias_to_q(100));
  EXPECT_EQ(0.6, ias_to_q(1));
}

TEST(Aerodynamics, q_to_ias) {
  EXPECT_EQ(100, q_to_ias(6000));
  EXPECT_EQ(1, q_to_ias(0.6));
}

} // namespace airball
