#ifndef CALIBRATION_SURFACE_H
#define CALIBRATION_SURFACE_H

#if defined(__cplusplus)
extern "C"
{
#endif

typedef struct axis_size_struct {
  uint16_t size;
  float step;
  float zero_offset;
} axis_size;

typedef struct calibration_surface_struct {
  axis_size x;
  axis_size y;
  
  float* data;
  
} calibration_surface;

float interpolate(const calibration_surface* c, const float x, const float y, int* err);

#if defined(__cplusplus)
}
#endif

#endif // CALIBRATION_SURFACE_H
