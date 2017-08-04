#ifndef AIRBALL_AERODYNAMICS_H
#define AIRBALL_AERODYNAMICS_H

namespace airball {

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

} // namespace airball

#endif // AIRBALL_AERODYNAMICS_H
