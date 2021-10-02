#include "pressures_to_airdata.h"
#include <math.h>

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
  float rpb_pos = math.abs(rpb);
  float signb = rpb < 0 ? -1.0 : 1.0;
  
  r.alpha = interpolate(cs_alpha, rpa, rpb_pos, err);
  if (*err != 0) { return r; }
  
  r.beta = interpolate(cs_beta, rpa, rpb_pos, err);
  if (*err != 0) { return r; }
  r.beta = r.beta * signb;
  
  float q_over_dp0 = interpolate(cs_q_over_dp0, rpa, rpb_pos, err);  
  if (*err != 0) { return r; }
  r.q = dp0 * q_over_dp0;

  return r;  
}
