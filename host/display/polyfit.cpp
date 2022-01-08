#include "polyfit.h"

#include <cmath>
#include <vector>
#include <eigen3/Eigen/QR>

// Implementation by Rahul Bhadani
// https://towardsdatascience.com/least-square-polynomial-fitting-using-c-eigen-package-c0673728bd01

namespace airball {

void polyfit(
    const std::vector<double> &t,
    const std::vector<double> &v,
    std::vector<double> &coeff,
    int order) {
  Eigen::MatrixXd T(t.size(), order + 1);
  Eigen::VectorXd V = Eigen::VectorXd::Map(&v.front(), v.size());
  Eigen::VectorXd result;

  assert(t.size() == v.size());
  assert(t.size() >= order + 1);

  for (size_t i = 0; i < t.size(); i++) {
    for (size_t j = 0; j < order + 1; j++) {
      T(i, j) = pow(t.at(i), j);
    }
  }
  result = T.householderQr().solve(V);
  coeff.resize(order + 1);
  for (int k = 0; k < order + 1; k++) {
    coeff[k] = result[k];
  }
}

} // namespace airball