#ifndef AIRBALL_DISPLAY_POLYFIT_H
#define AIRBALL_DISPLAY_POLYFIT_H

#include <vector>

namespace airball {

void polyfit(
    const std::vector<double> &x,
    const std::vector<double> &y,
    std::vector<double> &coeff,
    int order);

} // namespace airball

#endif // AIRBALL_DISPLAY_POLYFIT_H