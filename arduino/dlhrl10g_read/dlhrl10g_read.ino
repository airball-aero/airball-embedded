#include <Adafruit_BMP280.h>

#include <Wire.h>
extern "C" { 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

bool isBusy(byte status) {
  return (status & 0x20) != 0;
}

bool checkBusy(byte address) {
  Wire.requestFrom(address, 1);
  bool busy = isBusy(Wire.read());
  Wire.endTransmission();
  return busy;
}

void sendPressureSensorReadSingle(byte address) {
  Wire.beginTransmission(address);
  Wire.write(0xaa);  // command read single
  Wire.write(0x00);  // command byte ignored
  Wire.write(0x00);  // command byte ignored
  Wire.endTransmission();
}

unsigned long readPressureSensor(byte address) {
  while (checkBusy(address)) { }
  Wire.requestFrom(address, 7);
  Wire.read();  // status byte ignored
  byte b2 = Wire.read();  // pressure byte 2 (msb)
  byte b1 = Wire.read();  // pressure byte 1
  byte b0 = Wire.read();  // pressure byte 0 (lsb)
  Wire.read();  // temperature byte ignored
  Wire.read();  // temperature byte ignored
  Wire.read();  // temperature byte ignored
  Wire.endTransmission();

  unsigned long int_raw_value = 0;
  int_raw_value |= ((unsigned long) b2) << 16;
  int_raw_value |= ((unsigned long) b1) <<  8;
  int_raw_value |= ((unsigned long) b0);

  return int_raw_value;
}

double pressureTransferFunction(
    double raw_value,
    double zero_ref,
    double full_scale_ref,
    double fss) {
  return
      ((raw_value - zero_ref) / full_scale_ref)
      * ((double) 1.25)
      * fss;    
 }

double scalePressureReadingGage(unsigned long digital, double range) {
  double zero_ref = 0.1 * (double)(((unsigned long) 1) << 24);
  double full_scale_ref = ((unsigned long) 1) << 24;
  return pressureTransferFunction(
      (double) digital, 
      zero_ref, 
      full_scale_ref, 
      range);
}

double scalePressureReadingDiff(unsigned long digital, double range) {
  double zero_ref = 0.5 * (double)(((unsigned long) 1) << 24);
  double full_scale_ref = ((unsigned long) 1) << 24;
  return pressureTransferFunction(
      (double) digital,
      zero_ref,
      full_scale_ref,
      range * 2);
}

void sendMuxChipSelect(byte muxAddress, byte chipNumber) {
  if (chipNumber > 7) { return; }
  Wire.beginTransmission(muxAddress);
  Wire.write(1 << chipNumber);
  Wire.endTransmission();
}

void printValueGage(double value, double range, char symbol) {
  double cnt = value / range * 1000.0;
  Serial.print("|");
  for (int i = 0; i < cnt; i++) {
    Serial.print(symbol);
  }
  Serial.println();
}

void sendSentence(
     double baro,
     double temp,
     double dp0,
     double dpAlpha,
     double dpBeta) {
  Serial1.print(baro);
  Serial1.print(",");
  Serial1.print(temp);
  Serial1.print(",");  
  Serial1.print(dp0);
  Serial1.print(",");
  Serial1.print(dpAlpha);
  Serial1.print(",");
  Serial1.print(dpBeta);
  Serial1.println();
}

#define TCAADDR 0x70
 
void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission(); 
}

void tcascan() {
    Serial.println("\nTCAScanner ready!");
    
    for (uint8_t t=0; t<8; t++) {
      tcaselect(t);
      Serial.print("TCA Port #"); Serial.println(t);
 
      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCAADDR) continue;
      
        uint8_t data;
        if (! twi_writeTo(addr, &data, 0, 1, 1)) {
           Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
        }
      }
    }
    Serial.println("\ndone");  
}


Adafruit_BMP280 bmp;

void setup() {
    while (!Serial1);
    Wire.begin();
    Serial1.begin(9600);
    tcascan();
    sendMuxChipSelect(0x70, 0x00);
    bmp.begin();
}

#define PASCALS_PER_IN_H2O 249.08
#define PRESSURE_RANGE 10.0 * PASCALS_PER_IN_H2O

void loop() {
  sendMuxChipSelect(0x70, 0x01);
  sendPressureSensorReadSingle(0x29);
  sendMuxChipSelect(0x70, 0x02);
  sendPressureSensorReadSingle(0x29);
  sendMuxChipSelect(0x70, 0x03);
  sendPressureSensorReadSingle(0x29);

  sendMuxChipSelect(0x70, 0x00);
  double p0 = bmp.readPressure();
  double t = bmp.readTemperature();
  
  sendMuxChipSelect(0x70, 0x01);
  double dp0 = scalePressureReadingGage(
      readPressureSensor(0x29),
      PRESSURE_RANGE);
  
  sendMuxChipSelect(0x70, 0x02);
  double dpA = scalePressureReadingDiff(
      readPressureSensor(0x29),
      PRESSURE_RANGE);
 
  sendMuxChipSelect(0x70, 0x03);
  double dpB = scalePressureReadingDiff(
      readPressureSensor(0x29),
      PRESSURE_RANGE);

  sendSentence(p0, t, dp0, dpA, dpB);

  delay(50);
}
