// ADXL 345 calibration code, by SparkFun

#include <SparkFun_ADXL345.h>

ADXL345 adxl = ADXL345();             // Use when you need I2C

int AccelMinX = 0;
int AccelMaxX = 0;
int AccelMinY = 0;
int AccelMaxY = 0;
int AccelMinZ = 0;
int AccelMaxZ = 0; 

int accX = 0;
int accY = 0;
int accZ = 0;

int pitch = 0;
int roll = 0;

#define offsetX   0       // OFFSET values
#define offsetY   0
#define offsetZ   0

#define gainX     1     // GAIN factors
#define gainY     1
#define gainZ     1 

void setup() {
  Serial.begin(9600);
  Serial.println("SparkFun ADXL345 Accelerometer Breakout Calibration");
  Serial.println();

  adxl.powerOn();
  adxl.setRangeSetting(2);
}

void loop() {
  Serial.println("Send any character to display values.");
  while (!Serial.available()) {}
  Serial.println();

  int x,y,z;
  adxl.readAccel(&x, &y, &z);

  if(x < AccelMinX) AccelMinX = x;
  if(x > AccelMaxX) AccelMaxX = x;

  if(y < AccelMinY) AccelMinY = y;
  if(y > AccelMaxY) AccelMaxY = y;

  if(z < AccelMinZ) AccelMinZ = z;
  if(z > AccelMaxZ) AccelMaxZ = z;

  Serial.print("Accel Minimums: ");
  Serial.print(AccelMinX);
  Serial.print("  ");
  Serial.print(AccelMinY);
  Serial.print("  ");
  Serial.print(AccelMinZ);
  Serial.println();

  Serial.print("Accel Maximums: ");
  Serial.print(AccelMaxX);
  Serial.print("  ");
  Serial.print(AccelMaxY);
  Serial.print("  ");
  Serial.print(AccelMaxZ);
  Serial.println();
  Serial.println();

  while (Serial.available()) {
    Serial.read();
  }
}

