#include <stdint.h>
#include <stdio.h>

#include "calibration_surface.h"

const calibration_surface s0 = {
  .x = {
    .size = 2,
    .step = 1.0f,
    .zero_offset = 0.0f,
  },
  .y = {
    .size = 2,
    .step = 1.0f,
    .zero_offset = 0.0f,
  },
  .data = (float[]){
    1.0f,
    2.0f,
    3.0f,
    4.0f,
  },
};

int main(int argc, char**argv) {
  int err;
  printf("result = %f\n", interpolate(&s0, 1.5f, 0.5f, &err));
  printf("err = %d\n", err);
  printf("result = %f\n", interpolate(&s0, 10.0f, 10.0f, &err));
  printf("err = %d\n", err);  
}
