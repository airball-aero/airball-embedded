// ADXL 345 data reading code, by SparkFun.

#include <SparkFun_ADXL345.h>

#define SCALE 2 // Accepted values are 2g, 4g, 8g or 16g

ADXL345 adxl = ADXL345();

void setup(){
  Serial.begin(9600);
  adxl.powerOn();
  adxl.setRangeSetting(SCALE);
}

void loop(){
  int x,y,z;  
  adxl.readAccel(&x, &y, &z);
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(z); 
}
