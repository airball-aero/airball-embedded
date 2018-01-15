#include <gtest/gtest.h>
#include "interpolation_table.h"

TEST(InterpolationTable, TestSimpleTwoPoints) {
  airball::InterpolationTable t;
  t.put(3, 30);
  t.put(1, 10);
  EXPECT_NEAR(
      2,
      t.x(20),
      0.0001);
  EXPECT_TRUE(std::isnan(t.x( 5)));
  EXPECT_TRUE(std::isnan(t.x(35)));
}

constexpr double kX0 = 0;
constexpr double kX1 = 1;
constexpr int kNumPrecisionPoints = 1000;
constexpr int kNumTestPoints = 200;

TEST(InterpolationTable, TestQuadratic) {
  airball::InterpolationTable t;

  auto f = [](double x) {
    return x * x + x + 1;
  };

  for (int i = 0; i < kNumPrecisionPoints; i++) {
    double ratio = ((double) std::rand()) / ((double) RAND_MAX);
    double x = kX0 + ratio * (kX1 - kX0);
    t.put(x, f(x));
  }
  t.put(kX0, f(kX0));
  t.put(kX1, f(kX1));

  // Note that we do not test the endpoints (i == 0) and (i == kNumTestPoints)
  // since roundoff error at the endpoints is likely to cause that to be
  // interpreted as an extrapolation, for which InterpolationTable returns NaN.
  for (int i = 1; i < kNumTestPoints; i++) {
    double x = kX0 + ((double) i) / ((double) kNumTestPoints) * (kX1 - kX0);
    double y = f(x);
    EXPECT_NEAR(
        x,
        t.x(y),
        0.0001);
  }
}

TEST(InterpolationTable, TestCsv) {
  airball::InterpolationTable t;
  t.put(0, 0);
  t.put(1, 1);
  ASSERT_EQ(
      t.get_csv(),
      "0,0\n"
      "1,1\n");
}
