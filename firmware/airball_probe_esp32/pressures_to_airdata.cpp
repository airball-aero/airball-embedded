#include "pressures_to_airdata.h"

struct airdata_triple_struct pressures_to_airdata(const calibration_surface* cs_alpha,
						  const calibration_surface* cs_beta,
						  const calibration_surface* cs_q_over_dp0,
						  float dp0,
						  float dpa,
						  float dpb,
						  int* err) {
  airdata_triple r;

  float rpa = dpa / dp0;
  float rpb = dpb / dp0;
  float signb;
  
  if (rpb < 0) {
    rpb = -rpb;
    signb = -1.0;
  } else {
    signb = 1.0;
  }

  r.alpha = interpolate(cs_alpha, rpa, rpb, err);
  if (*err != 0) { return r; }
  
  r.beta = interpolate(cs_beta, rpa, rpb, err);
  if (*err != 0) { return r; }
  r.beta *= signb;
    
  float q_over_dp0 = interpolate(cs_q_over_dp0, rpa, rpb, err);  
  if (*err != 0) { return r; }
  r.q = dp0 * q_over_dp0;

  return r;  
}
