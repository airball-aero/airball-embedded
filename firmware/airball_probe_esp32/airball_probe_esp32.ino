#define LED_BUILTIN 19

#include "calibration_surface.h"
#include "v2_probe_calibration.h"

void setup() {
  Serial.begin(115200);
}


constexpr float dp_min = -1.25f;
constexpr float dp_max =  1.25;
constexpr float dp_step = 0.01f;

float dp = 0.0f;

void update_dp() {
  dp += dp_step;
  if (dp > dp_max) {
    dp = dp_min;
  }
}

void loop() {
  int err = 0;

  Serial.printf("dp = %10.6f", dp);
  Serial.print(" -> ");

  float alpha = interpolate(&v2_probe_alpha, dp, dp, &err);
  if (err != 0) { Serial.println("error alpha"); return; }
  float beta = interpolate(&v2_probe_beta, dp, dp, &err);
  if (err != 0) { Serial.println("error beta"); return; }
  float q_over_dp0 = interpolate(&v2_probe_q_over_dp0, dp, dp, &err);  
  if (err != 0) { Serial.println("error q_over_dp0"); return; }

  Serial.printf("alpha=%10.6f", alpha);
  Serial.print(" ");
  Serial.printf("beta=%10.6f", beta);
  Serial.print(" ");
  Serial.printf("q_over_dp0=%10.6f", q_over_dp0);
  Serial.print(" ");
  Serial.println();

  update_dp();

  delay(5);
}
