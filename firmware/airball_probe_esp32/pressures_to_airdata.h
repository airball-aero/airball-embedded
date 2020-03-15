#include "calibration_surface.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef struct airdata_triple_struct {
  float alpha;
  float beta;
  float q;
} airdata_triple;

struct airdata_triple_struct pressures_to_airdata(const calibration_surface* cs_alpha,
						  const calibration_surface* cs_beta,
						  const calibration_surface* cs_q_over_dp0,
						  float dp0,
						  float dpa,
						  float dpb,
						  int* err);
#if defined(__cplusplus)
}
#endif
