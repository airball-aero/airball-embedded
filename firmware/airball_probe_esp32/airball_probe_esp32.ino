#define LED_BUILTIN 19

#include "calibration_surface.h"
#include "v2_probe_calibration.h"

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

constexpr int blink_interval = 15;
int n = 0;
bool state = false;

typedef struct airdata_triple_struct {
  float alpha;
  float beta;
  float q;
} airdata_triple;

struct airdata_triple_struct pressures_to_airdata(float dp0, float dpa, float dpb, int* err) {
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

  r.alpha = interpolate(&v2_probe_alpha, rpa, rpb, err);
  if (*err != 0) { return r; }
  
  r.beta = interpolate(&v2_probe_beta, rpa, rpb, err);
  if (*err != 0) { return r; }
  r.beta *= signb;
    
  float q_over_dp0 = interpolate(&v2_probe_q_over_dp0, rpa, rpb, err);  
  if (*err != 0) { return r; }
  r.q = dp0 * q_over_dp0;

  return r;  
}

constexpr float dp_min = -1.35f;
constexpr float dp_max =  1.35f;
constexpr float dp_step = 0.001f;

float dp = 0.0f;

void update_dp() {
  dp += dp_step;
  if (dp > dp_max) {
    dp = dp_min;
  }
}

void loop() {
  int err = 0;

  airdata_triple airdata = pressures_to_airdata(1.0, dp, dp, &err);

  Serial.printf("dp = %10.6f", dp);
  Serial.print(" -> ");
  if (err != 0) {
    Serial.println("error");
  } else {
    Serial.printf("alpha=%10.6f", airdata.alpha);
    Serial.print(" ");
    Serial.printf("beta=%10.6f", airdata.beta);
    Serial.print(" ");
    Serial.printf("q=%10.6f", airdata.q);
    Serial.println();
  }
  
  update_dp();

  n++;
  if (n > blink_interval) {
    n = 0;
    // digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
    state = !state;
  }

  delay(5);
}
