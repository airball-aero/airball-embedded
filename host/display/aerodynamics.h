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

#ifndef AIRBALL_AERODYNAMICS_H
#define AIRBALL_AERODYNAMICS_H

namespace airball {

constexpr double QNH_STANDARD = 101.3e+03; // pascals

/**
 * Compute the dynamic pressure given indicated air speed.
 *
 * @param ias indicated air speed.
 * @return dynamic pressure.
 */
double ias_to_q(double ias);

/**
 * Compute the indicated air speed given dynamic pressure.
 *
 * @param q dynamic pressure.
 * @return indicated air speed.
 */
double q_to_ias(double q);

/**
 * Compute the density of dry air at a given barometric condition.
 *
 * @param p barometric pressure.
 * @param t temperature.
 * @return density of dry air.
 */
double dry_air_density(double p, double t);

/**
 * Compute the true air speed given dynamic pressure and barometric data.
 *
 * @param q dynamic pressure.
 * @param p barometric pressure.
 * @param t temperature.
 * @return true air speed.
 */
double q_to_tas(double q, double p, double t);

/**
 * Compute the altitude given the barometric pressure.
 *
 * @param t the ambient temperature.
 * @param p barometric pressure at this point.
 * @param qnh barometer setting at this point (pressure at sea level).
 * @return altitude.
 */
double pressure_to_altitude(double t, double p, double qnh);

} // namespace airball

#endif // AIRBALL_AERODYNAMICS_H
