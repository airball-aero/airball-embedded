#include <stdint.h>
#include "calibration_surface.h"
#include <stdio.h>

uint16_t idx(const axis_size *sz, const float v, int* err) {
  // printf("v = %f\n", v);
  // printf("sz->zero_offset = %f\n", sz->zero_offset);
  // printf("sz->step = %f\n", sz->step);
  // printf("sz->size = %u\n", sz->size);
  int32_t i = int((v + sz->zero_offset) / sz->step);
  if (i < 0 || i >= sz->size) { *err = -1; }
  // printf("%f -> %d, err = %d\n", v, i, *err);
  return (uint16_t)i;
}

float get(const calibration_surface* c, const uint16_t idx_x, const uint16_t idx_y) {
  return c->data[c->y.size * idx_x + idx_y];
}

float lineint(float x1, float y1, float x2, float y2, float x) {
  return y1 + (x - x1) / (x2 - x1) * (y2 - y1);
}

float interpolate(const calibration_surface* c, const float x, const float y, int* err) {
  *err = 0;
  
  uint16_t ix = idx(&(c->x), x, err);
  if (*err != 0) { return 0.0f; }
  
  uint16_t iy = idx(&(c->y), y, err);
  if (*err != 0) { return 0.0f; }
  
  float at_y0 = lineint(c->x.step * (ix    ), get(c, ix    , iy    ),
			c->x.step * (ix + 1), get(c, ix + 1, iy    ),
			x);
  float at_y1 = lineint(c->x.step * (ix    ), get(c, ix    , iy + 1),
			c->x.step * (ix + 1), get(c, ix + 1, iy + 1),
			x);
  float at_md = lineint(c->y.step * (iy    ), at_y0,
			c->y.step * (iy + 1), at_y1,
			y);
  return at_md;
}
