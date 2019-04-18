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

#include "airdata.h"

#include "gtest/gtest.h"

namespace airball {

std::unique_ptr<airdata_sample> airdata(double baro,
                                        double temperature,
                                        double dp0,
                                        double dpA,
                                        double dpB) {
  return std::unique_ptr<airdata_sample>(new airdata_sample(
      std::chrono::system_clock::now(),
      128,
      999,
      baro,
      temperature,
      dp0,
      dpA,
      dpB));
}

void update(Airdata *ad, const airdata_sample *d, const double qnh) {
  airdata_sample sample(
      std::chrono::system_clock::now(),
      128,
      999,
      0,
      0,
      0,
      0,
      0);
  for (int i = 0; i < 1000; i++) {
    ad->update(&sample, qnh);
  }
  ad->update(d, qnh);
}

TEST(Airdata, simple_zero_condition) {
  Airdata ad;
  update(
      &ad,
      airdata(
          1013.25,
          15.0,
          6000,
          0.0,
          0.0).get(),
      1013.25);
  EXPECT_NEAR(0.0, ad.alpha(), 0.001);
  EXPECT_NEAR(0.0, ad.beta(), 0.001);
}

TEST(Airdata, ias_test) {
  Airdata ad;
  update(
      &ad,
      airdata(
          1013.25,
          15.0,
          6125,
          0.0,
          0.0).get(),
      1013.25);
  EXPECT_NEAR(100, ad.ias(), 0.02);
}

TEST(Airdata, tas_test) {
  Airdata ad;
  // At standard pressure, T=226.85, dry air density = 0.706
  // At that density, Q=3530, true airspeed=100
  // TODO: Add a test for nonstandard pressure
  update(
      &ad,
      airdata(
          1013.25,
          15.0,
          3530,
          0.0,
          0.0).get(),
      1013.25);
  EXPECT_NEAR(100, ad.tas(), 0.02);
}

// To a very rough approximation, for small deflctions, we know that the
// nondimensional pressure difference between top and bottom holes is related
// to the angle of attack via a scaling factor (angle in radians):
//   alpha = (dpAlpha / dp0) * .2

TEST(Airdata, alpha_test) {
  Airdata ad;
  update(
      &ad,
      airdata(
          1013.25,
          15.0,
          100,
          10,
          0).get(),
      1013.25);
  EXPECT_NEAR(-0.02, ad.alpha(), 0.01);
  EXPECT_NEAR(0, ad.beta(), 0.01);
}

TEST(Airdata, beta_test) {
  Airdata ad;
  update(
      &ad,
      airdata(
          1013.25,
          15.0,
          100,
          0,
          10).get(),
      1013.25);
  EXPECT_NEAR(0, ad.alpha(), 0.01);
  EXPECT_NEAR(0.02, ad.beta(), 0.01);
}

TEST(Airdata, alpha_beta_test) {
  Airdata ad;
  update(
      &ad,
      airdata(
          1013.25,
          15.0,
          6125,
          612.5,
          612.5).get(),
      1013.25);
  EXPECT_NEAR(-0.0222, ad.alpha(), 0.01);
  EXPECT_NEAR(0.0222, ad.beta(), 0.01);
}

TEST(Airdata, ias_deflected_test) {
  Airdata ad;
  update(
      &ad,
      airdata(
          1013.25,
          15.0,
          6125,
          612.5,
          612.5).get(),
      1013.25);
  EXPECT_NEAR(100.111, ad.ias(), 0.01);
}

TEST(Airdata, altitude_test_0) {
  Airdata ad;
  update(
      &ad,
      airdata(
          1013.25,
          25.0,
          0,
          0,
          0).get(),
      1013.25);
  EXPECT_NEAR(0, ad.altitude(), 0.01);
}

TEST(Airdata, altitude_test_low) {
  Airdata ad;
  update(
      &ad,
      airdata(
          1005.00,
          25.0,
          0,
          0,
          0).get(),
      1013.25);
  EXPECT_NEAR(71.39, ad.altitude(), 0.01);
}

TEST(Airdata, altitude_test_mid) {
  Airdata ad;
  update(
      &ad,
      airdata(
          900.00,
          25.0,
          0,
          0,
          0).get(),
      1013.25);
  EXPECT_NEAR(1045.91, ad.altitude(), 0.01);
}

TEST(Airdata, altitude_test_high) {
  Airdata ad;
  update(
      &ad,
      airdata(
          50.00,
          25.0,
          0,
          0,
          0).get(),
      1013.25);
  EXPECT_NEAR(35431.44, ad.altitude(), 0.01);
}

TEST(Airdata, altitude_test_low_baro) {
  Airdata ad;
  update(
      &ad,
      airdata(
          1005.00,
          25.0,
          0,
          0,
          0).get(),
      1010.00);
  EXPECT_NEAR(43.32, ad.altitude(), 0.01);
}

TEST(Airdata, altitude_test_high_baro) {
  Airdata ad;
  update(
      &ad,
      airdata(
          500.00,
          25.0,
          0,
          0,
          0).get(),
      1010.00);
  EXPECT_NEAR(6563.95, ad.altitude(), 0.01);
}

} // namespace airball
