#include <stdint.h>
#include <stdio.h>

#include "calibration_surface.h"

//         X | Y:  -5.0f [0]    -3.0f [1]    -1.0f [2]
// -----------------------------------------------------------
// -2.0f [0] |           1.0          2.0          3.0
// -1.0f [1] |           4.0          5.0          6.0
//  0.0f [2] |           7.0          8.0          9.0
//  1.0f [3] |          10.0         11.0         12.0
//  2.0f [4] |          13.0         14.0         15.0

const calibration_surface s0 = {
  .x = {
    .size = 5,
    .step = 1.0f,
    .zero_offset = 2.0f,
  },
  .y = {
    .size = 3,
    .step = 2.0f,
    .zero_offset = 5.0f,
  },
  .data = (float[]){
     1.0f,  2.0f,  3.0f,
     4.0f,  5.0f,  6.0f,
     7.0f,  8.0f,  9.0f,
    10.0f, 11.0f, 12.0f,
    13.0f, 14.0f, 15.0f,     
  },
};

#define TOLERANCE 0.0005f

void check_near(float x, float y) {
  float diff = y - x;
  if (diff < 0) { diff = -diff; }
  if (diff > TOLERANCE) {
    printf("ERROR: (%10.6f, %10.6ff) differ by more than %10.6f\n", x, y, TOLERANCE);
  }
}

void check(const float x, const float y, const float z) {
  int err = 0;
  float zo = interpolate(&s0, x, y, &err);
  if (err != 0) { printf("ERROR: %10.6f %10.6f caused error\n", x, y); }
  check_near(zo, z);
}

int main(int argc, char**argv) {
  check(-2.0f, -5.0f,  1.00f);
  check(-1.0f, -3.0f,  5.00f);
  check( 2.0f, -3.0f, 14.00f);
  check(-1.5f, -4.0f,  3.00f);
  check( 2.0f, -1.0f, 15.00f);
  check( 1.5f, -2.0f, 13.00f);  
}
