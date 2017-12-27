#include <TimerThree.h>

#include <SparkFunTMP102.h>

#include <Wire.h>
extern "C" { 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

bool debug = true;

unsigned long timer_calls = 0;


void log(const char* s) {
  if (debug) {
    Serial1.print("# ");
    Serial1.println(s);
  }
}

bool isBusy(byte status) {
  return (status & 0x20) != 0;
}

bool checkBusy(byte address) {
  Wire.requestFrom(address, (byte)1);
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

unsigned long readPressureSensor(byte bus, byte address) {
  char msg[128];
  while (checkBusy(address)) {
    sprintf(msg, ". 0x%02x @ 0x%02x", bus, address);
    log(msg);
    delay(1);
  }
  Wire.requestFrom(address, (byte)7);
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

double celsiusToKelvin(double celsius) {
  return celsius + 273.15;
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
  // Serial1.print(" -- ");
  // Serial1.print(timer_calls);
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
    Serial1.println("# TCAScanner ready!");
    
    for (uint8_t t=0; t<5; t++) {
      tcaselect(t);
      Serial1.print("# TCA Port #"); Serial1.println(t);

      continue; // FOO
      
      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCAADDR) continue;
      
        uint8_t data;
        if (! twi_writeTo(addr, &data, 0, 1, 1)) {
           Serial1.print("#   Found I2C 0x");  Serial1.println(addr,HEX);
        }
      }
    }
    Serial1.println("# done");  
}

void setup() {
  while (!Serial1) {
    delay(10);
  }
  Serial1.begin(9600);

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  delay(10);

  log("Serial established");    
  Wire.begin();
  log("Wire begin completed");
  if (debug) { tcascan(); }

/*
  Timer3.initialize();
  Timer3.setPeriod(500 * 1000);
  Timer3.attachInterrupt(timerCallback);
  */
}

const unsigned long period_ms = 500;
unsigned long last_ms = 0;

/*

ISR(TIMER0_COMPA_vect) {
  unsigned long current_ms = millis();
  if (current_ms - last_ms > period_ms) {
    timer_calls++;
    last_ms = current_ms;
    // Serial1.println("# signal timer0");    
  }
}

*/

void timerCallback() {
  Serial1.println("# timer");
}

#define PASCALS_PER_IN_H2O 249.08
#define PRESSURE_RANGE 10.0 * PASCALS_PER_IN_H2O

TMP102 tempSensor(0x48);

void loop() {
  sendMuxChipSelect(0x70, 0x00);
  sendPressureSensorReadSingle(0x29);

  sendMuxChipSelect(0x70, 0x02);
  sendPressureSensorReadSingle(0x29);

  sendMuxChipSelect(0x70, 0x03);
  sendPressureSensorReadSingle(0x29);

  sendMuxChipSelect(0x70, 0x00);
  double dp0 = scalePressureReadingGage(
      readPressureSensor(0x00, 0x29),
      PRESSURE_RANGE);

  sendMuxChipSelect(0x70, 0x02);
  double dpAlpha = scalePressureReadingDiff(
      readPressureSensor(0x02, 0x29),
      PRESSURE_RANGE);

  sendMuxChipSelect(0x70, 0x03);
  double dpBeta = scalePressureReadingDiff(
      readPressureSensor(0x03, 0x29),
      PRESSURE_RANGE);

  sendMuxChipSelect(0x70, 0x04);
  double t = celsiusToKelvin(tempSensor.readTempC());

  sendSentence(0, t, dp0, dpAlpha, dpBeta);

  delay(50);
}

void xloop() {}

