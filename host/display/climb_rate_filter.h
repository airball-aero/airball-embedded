#ifndef AIRBALL_CLIMBRATEFILTER_H_
#define AIRBALL_CLIMBRATEFILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 20 Hz

* 0 Hz - 0.125 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = 4.140172463045487 dB

* 0.25 Hz - 10 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -40.071971995404276 dB

*/

#define CLIMBRATEFILTER_TAP_NUM 203

typedef struct {
  double history[CLIMBRATEFILTER_TAP_NUM];
  unsigned int last_index;
} climbrateFilter;

void climbrateFilter_init(climbrateFilter* f);
void climbrateFilter_put(climbrateFilter* f, double input);
double climbrateFilter_get(climbrateFilter* f);

#endif // AIRBALL_CLIMBRATEFILTER_H_