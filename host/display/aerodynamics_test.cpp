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

#include "aerodynamics.h"

#include "gtest/gtest.h"

namespace airball {

TEST(Aerodynamics, ias_to_q) {
  EXPECT_EQ(6125, ias_to_q(100));
  EXPECT_EQ(0.6125, ias_to_q(1));
}

TEST(Aerodynamics, q_to_ias) {
  EXPECT_EQ(100, q_to_ias(6125));
  EXPECT_EQ(1, q_to_ias(0.6125));
}

TEST(Aerodynamics, dry_air_density) {
  EXPECT_NEAR(0.0117, dry_air_density(1000, 25), 0.001);
  EXPECT_NEAR(1.225, dry_air_density(101.3e+03, 15), 0.001);
}

TEST(Aerodynamics, q_to_tas) {
  EXPECT_NEAR(100, q_to_tas(6125, 101.3e+03, 15), .1);
  EXPECT_NEAR(10, q_to_tas(61.25, 101.3e+03, 15), 0.01);
}

} // namespace airball
