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


} // namespace airball

#endif // AIRBALL_AERODYNAMICS_H
