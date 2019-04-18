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

#include "units.h"

#include <math.h>

namespace airball {

constexpr static double kDensityAirStandard = 1.225;
constexpr static double kDryAirGasConstant = 287.058;

double ias_to_q(double ias) {
  return 0.5 * kDensityAirStandard * pow(ias, 2);
}

double q_to_ias(double q) {
  return sqrt(2.0 * q / kDensityAirStandard);
}

double dry_air_density(double p, double t) {
  return p / (kDryAirGasConstant * celsius_to_kelvin(t));
}

double q_to_tas(double q, double p, double t) {
  return sqrt(2.0 * q / dry_air_density(p, t));
}

static constexpr double kAltK = 1.313e-05;
static constexpr double kAltN = 0.1903;
static constexpr double kAltPb = 29.92126;

// The formulae are given in English units. To maintain a consistent API
// throughout our system, the function inputs and outputs are in SI units. The
// tradeoff is we do a few more unit conversions than needed, in return for a
// hopefully less error-prone set of APIs.
double pressure_to_altitude(double t, double p, double qnh) {
  double ratio = (qnh / p);
  double t_kelvin = t + 273.15;
  double power = 1 / 5.257;
  return (pow(ratio, power) - 1) * t_kelvin / 0.0065;
}

} // namespace airball
