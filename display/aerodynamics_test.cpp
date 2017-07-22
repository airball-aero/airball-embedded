#include "aerodynamics.h"

#include "gtest/gtest.h"

namespace airball {

TEST(Aerodynamics, ias_to_q) {
  EXPECT_EQ(6000, ias_to_q(100));
  EXPECT_EQ(0.6, ias_to_q(1));
}

TEST(Aerodynamics, q_to_ias) {
  EXPECT_EQ(100, q_to_ias(6000));
  EXPECT_EQ(1, q_to_ias(0.6));
}

TEST(Aerodynamics, gage_pressure_at_point) {
  EXPECT_NEAR(
      0.4828400941,
      gage_pressure_at_point(1, 0.5),
      0.0001);
}

} // namespace airball
