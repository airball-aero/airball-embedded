/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018, Jeremy Cole
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Define the appropriate probe sensor board version (3, 4):
#define AIRBALL_PROBE_VERSION 4
#define HAVE_30INH2O_SENSORS

// Define AIRBALL_DEBUG to wait for USB serial connection and output (many) debug
// messages to the console.
//#define AIRBALL_DEBUG

#if AIRBALL_PROBE_VERSION == 4
#define HAVE_TMP275
#define HAVE_DS2782
#endif

#include <Wire.h>

#include <TimerOne.h>
#include <TI_TCA9548A.h>
#include <AllSensors_DLHR.h>
#include <AllSensors_DLV.h>
#include <Temperature_LM75_Derived.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "XBee.h"

#ifdef HAVE_DS2782
#include <Maxim_DS2782.h>
#define BMP_VERSION_USER_EEPROM_OFFSET 0
#define BMP_VERSION 1
#define BMP_RSGAIN  715
#define BMP_RSNSP   67
#define BMP_VCHG    210
#define BMP_IMIN    6
#define BMP_VAE     154
#define BMP_IAE     6
#endif

// Which pin is the TCA9548A reset pin tied to?
#define MUX_RESET 4

// The channel configuration for the sensors.
#define MUX_CHANNEL_dp0   0
#define MUX_CHANNEL_baro  1
#define MUX_CHANNEL_dpA   2
#define MUX_CHANNEL_dpB   3
#define MUX_CHANNEL_oat   4
#define MUX_CHANNEL_bm    7

// How frequently (in uS) should measurements be taken?
#define MEASUREMENT_INTERVAL_US 50000

// At what point should the measurement slot counter be wrapped around? Ideally should be
// a multiple of 60 so that it wraps around on the minute.
#define MEASUREMENT_SLOT_WRAPAROUND 240

// Report once a second with a 50ms measurement interval.
#define OAT_REPORT_INTERVAL 20

#ifdef HAVE_DS2782
// How often (in MEASUREMENT_INTERVAL_US units) should the battery management reporting be sent?
#define BM_REPORT_INTERVAL 40
#endif

// The I2C multiplexer.
TI_TCA9548A mux(&Wire);

// All AllSensors DLHR pressure sensors.
#ifdef HAVE_30INH2O_SENSORS
AllSensors_DLHR_L30G_8 dp0(&Wire);
AllSensors_DLHR_L30D_8 dpA(&Wire);
AllSensors_DLHR_L30D_8 dpB(&Wire);
#else
AllSensors_DLHR_L10G_8 dp0(&Wire);
AllSensors_DLHR_L10D_8 dpA(&Wire);
AllSensors_DLHR_L10D_8 dpB(&Wire);
#endif // HAVE_30INH2O_SENSORS

AllSensors_DLV_015A baro(&Wire);

// The OAT sensor.

float oat_temperature;

#ifdef HAVE_TMP275
TI_TMP275 oat(&Wire);
#else
TI_TMP102 oat(&Wire);
#endif

OneWire onewire_bus(A5);
DallasTemperature ext_oat_sensors(&onewire_bus);
DeviceAddress ext_oat_address;
bool ext_oat_present = false;

#ifdef HAVE_DS2782
Maxim_DS2782 bm(&Wire, Maxim_DS2782::DEFAULT_I2C_ADDRESS, 0.015);
#endif

// The XBee radio to send telemetry back.
XBee radio(&Serial1);

// A global variable and callback function to track whether the timer has fired and it's time
// to collect a new measurement.
volatile boolean measurement_requested = false;
void callbackReadyForMeasurement() {
  measurement_requested = true;
}

void appendString(char *buf, char const *str, char const *delim) {
  strcat(buf, str);
  if (delim) strcat(buf, delim);
}

// Append a float value to a string, with an optional (string) delimiter following it.
char float_buf[10];
void appendFloat(char *buf, float value, char const *delim) {
  dtostrf(value, 0, 2, float_buf);
  strcat(buf, float_buf);
  if (delim) strcat(buf, delim);
}

void appendInteger(char *buf, unsigned long seq, char const *delim) {
  sprintf(float_buf, "%ld", seq);
  strcat(buf, float_buf);
  if (delim) strcat(buf, delim);
}

// Construct a telemetry sentence and send it to the provided stream (typically a Serial object).
char sentence[128];

void sendAirDataSentence(unsigned long seq, float baro, float oat, float dp0, float dpA, float dpB) {
  // Start with an empty string.
  sentence[0] = 0;

  // Append all variables to the sentence buffer, separated by commas and ended by a CR/NL.
  appendString(sentence, "$A", ",");
  appendInteger(sentence, seq, ",");
  appendFloat(sentence, baro, ",");
  appendFloat(sentence, oat, ",");
  appendFloat(sentence, dp0, ",");
  appendFloat(sentence, dpA, ",");
  appendFloat(sentence, dpB, "\r\n");

  // Write the sentence to the stream.
  radio.send_packet(0x8888, sentence, strlen(sentence));
}

#ifdef HAVE_DS2782
void sendBatterySentence(unsigned long seq, float bm_V, float bm_mA, float bm_capacity_mAh, float bm_capacity_pct) {
  // Start with an empty string.
  sentence[0] = 0;

  // Append all variables to the sentence buffer, separated by commas and ended by a CR/NL.
  appendString(sentence, "$B", ",");
  appendInteger(sentence, seq, ",");
  appendFloat(sentence, bm_V, ",");
  appendFloat(sentence, bm_mA, ",");
  appendFloat(sentence, bm_capacity_mAh, ",");
  appendFloat(sentence, bm_capacity_pct, "\r\n");

  // Write the sentence to the stream.
  radio.send_packet(0x8888, sentence, strlen(sentence));
}
#endif

uint8_t measurement_slot = 0;

unsigned long airdata_seq = 0;
unsigned long battery_seq = 0;

// Collect data from all sensors and send the data.
void completeMeasurementAndReport() {
  // Request start of measurements from all sensors, to allow them all to run concurrently.
  mux.selectChannel(MUX_CHANNEL_dp0);
  dp0.startMeasurement();

  mux.selectChannel(MUX_CHANNEL_dpA);
  dpA.startMeasurement();

  mux.selectChannel(MUX_CHANNEL_dpB);
  dpB.startMeasurement();

  // Collect the OAT while pressure measurements are still in progress.
  if (ext_oat_present) {
    if (measurement_slot % OAT_REPORT_INTERVAL == 0) {
      // Get the result of the measurement that was started earlier.
      oat_temperature = ext_oat_sensors.getTempC(ext_oat_address);
      // Kick off a new measurement.
      ext_oat_sensors.requestTemperaturesByAddress(ext_oat_address);
    }
  } else {
    // Use the on-board temperature sensor.
    oat_temperature = oat.readTemperatureC();
  }

  // Something is racy between TMP102 and the pressure sensor reading sequence; moving too quickly
  // results in occasional bad data from the first pressure sensor.
  delay(1);

  // Read all pressure sensors serially; the first will take longer for the measurement to complete
  // but the others should complete more quickly (since some time has already been taken).
  mux.selectChannel(MUX_CHANNEL_dp0);
  dp0.readData(true);

  mux.selectChannel(MUX_CHANNEL_dpA);
  dpA.readData(true);

  mux.selectChannel(MUX_CHANNEL_dpB);
  dpB.readData(true);

  mux.selectChannel(MUX_CHANNEL_baro);
  baro.readData();

  // Send the data sentence to the display.
  sendAirDataSentence(airdata_seq++, baro.pressure, oat_temperature, dp0.pressure, dpA.pressure, dpB.pressure);

#ifdef HAVE_DS2782
  if(measurement_slot % BM_REPORT_INTERVAL == 0) {
    float bm_V = bm.readVoltage();
    float bm_mA = bm.readCurrent();
    float bm_capacity_mAh = bm.readRemainingActiveAbsoluteCapacity();
    float bm_capacity_pct = (float)bm.readRemainingActiveRelativeCapacity() / 100.0;

    sendBatterySentence(battery_seq++, bm_V, bm_mA, bm_capacity_mAh, bm_capacity_pct);
  }
#endif

  ++measurement_slot %= MEASUREMENT_SLOT_WRAPAROUND;
}

char command_buf[128];
char *command_buf_p;

void clearCommandBuffer() {
  memset(command_buf, 0, sizeof(command_buf));
  command_buf_p = command_buf;
}

void handleCommand(char *command) {
  if (strncmp(command, "?", 1) == 0) {
    Serial.print("OK; Airball Sensor Board v");
    Serial.println(AIRBALL_PROBE_VERSION);
    return;
  }

#ifdef HAVE_DS2782
  if (strncmp(command, "BMS?", 4) == 0) {
      Serial.print("OK; ");

      Serial.print("VOLT=");
      Serial.print(bm.readVoltage());

      Serial.print(", CURRENT(mA)=");
      Serial.print(bm.readCurrent());

      Serial.print(", CURRENT(mV)=");
      Serial.print(bm.readCurrentSenseResistorVoltage());

      Serial.print(", TEMP=");
      Serial.print(bm.readTemperature());

      Maxim_DS2782::StatusRegister status = bm.readStatus();
      Serial.print(", CHGTF=");
      Serial.print(status.CHGTF);
      Serial.print(", AEF=");
      Serial.print(status.AEF);
      Serial.print(", SEF=");
      Serial.print(status.SEF);
      Serial.print(", LEARNF=");
      Serial.print(status.LEARNF);
      Serial.print(", UVF=");
      Serial.print(status.UVF);
      Serial.print(", PORF=");
      Serial.print(status.PORFx);
      Serial.println();
      return;
  }

  if (strncmp(command, "BMC?", 4) == 0) {
      Serial.print("OK; ");

      Serial.print("RAAC=");
      Serial.print(bm.readRemainingActiveAbsoluteCapacity());

      Serial.print(", RSAC=");
      Serial.print(bm.readRemainingStandbyAbsoluteCapacity());

      Serial.print(", RARC=");
      Serial.print(bm.readRemainingActiveRelativeCapacity());

      Serial.print(", RSRC=");
      Serial.print(bm.readRemainingStandbyRelativeCapacity());

      Serial.println();

      return;
  }

  if (strncmp(command, "BMP!", 4) == 0) {
      bm.doCopyData(1);
      Serial.print("OK; Battery management parameters stored.");
      return;
  }

  if (strncmp(command, "BMP?", 4) == 0) {
    Serial.print("OK; ");

    Serial.print("VERSION=");
    Serial.print(bm.readUserEEPROM_uint16(BMP_VERSION_USER_EEPROM_OFFSET));

    Serial.print(", RSGAIN=");
    Serial.print(bm.readRSGAIN());

    Serial.print(", RSNSP=");
    Serial.print(bm.readRSNSP());

    Serial.print(", VCHG=");
    Serial.print(bm.readVCHG());

    Serial.print(", IMIN=");
    Serial.print(bm.readIMIN());

    Serial.print(", VAE=");
    Serial.print(bm.readVAE());

    Serial.print(", IAE=");
    Serial.print(bm.readIAE());

    Serial.println();

    return;
  }

  if (strncmp(command, "BMP.VCHG?", 9) == 0) {
    Serial.print("OK; BMP.VCHG=");
    Serial.println(bm.readVCHG());
    return;
  }

  if (strncmp(command, "BMP.VCHG=", 9) == 0) {
    uint16_t value = atoi(command+9);
    if (value <= 255) {
      bm.writeVCHG((uint8_t)value);
      Serial.print("OK; BMP.VCHG=");
      Serial.println(value);
    } else {
      Serial.println("ERROR; Value out of range.");
    }
    return;
  }

  if (strncmp(command, "BMP.IMIN?", 9) == 0) {
    Serial.print("OK; BMP.IMIN=");
    Serial.println(bm.readIMIN());
    return;
  }

  if (strncmp(command, "BMP.IMIN=", 9) == 0) {
    uint16_t value = atoi(command+9);
    if (value <= 255) {
      bm.writeIMIN((uint8_t)value);
      Serial.print("OK; BMP.IMIN=");
      Serial.println(value);
    } else {
      Serial.println("ERROR; Value out of range.");
    }
    return;
  }

  if (strncmp(command, "BMP.VAE?", 8) == 0) {
    Serial.print("OK; BMP.VAE=");
    Serial.println(bm.readVAE());
    return;
  }

  if (strncmp(command, "BMP.VAE=", 8) == 0) {
    uint16_t value = atoi(command+8);
    if (value <= 255) {
      bm.writeVAE((uint8_t)value);
      Serial.print("OK; BMP.VAE=");
      Serial.println(value);
    } else {
      Serial.println("ERROR; Value out of range.");
    }
    return;
  }

  if (strncmp(command, "BMP.IAE?", 8) == 0) {
    Serial.print("OK; BMP.IAE=");
    Serial.println(bm.readIAE());
    return;
  }

  if (strncmp(command, "BMP.IAE=", 8) == 0) {
    uint16_t value = atoi(command+8);
    if (value <= 255) {
      bm.writeIAE((uint8_t)value);
      Serial.print("OK; BMP.IAE=");
      Serial.println(value);
    } else {
      Serial.println("ERROR; Value out of range.");
    }
    return;
  }

  if (strncmp(command, "BMP.RSNSP?", 10) == 0) {
    Serial.print("OK; BMP.RSNSP=");
    Serial.println(bm.readRSNSP());
    return;
  }

  if (strncmp(command, "BMP.RSNSP=", 10) == 0) {
    uint16_t value = atoi(command+10);
    if (value <= 255) {
      bm.writeRSNSP((uint8_t)value);
      Serial.print("OK; BMP.RSNSP=");
      Serial.println(value);
    } else {
      Serial.println("ERROR; Value out of range.");
    }
    return;
  }

  if (strncmp(command, "BMP.FRSGAIN?", 12) == 0) {
    Serial.print("OK; BMP.FRSGAIN=");
    Serial.println(bm.readFRSGAIN());
    return;
  }

  if (strncmp(command, "BMP.RSGAIN?", 11) == 0) {
    Serial.print("OK; BMP.RSGAIN=");
    Serial.println(bm.readRSGAIN());
    return;
  }

  if (strncmp(command, "BMP.RSGAIN=", 11) == 0) {
    uint16_t value = atoi(command+11);
    if (value <= 2047) {
      bm.writeRSGAIN(value);
      Serial.print("OK; BMP.RSGAIN=");
      Serial.println(value);
    } else {
      Serial.println("ERROR; Value out of range.");
    }
    return;
  }
#endif

  Serial.println("ERROR; Unknown command.");
}

void configureOneWire() {
  ext_oat_sensors.begin();

  if (ext_oat_sensors.getAddress(ext_oat_address, 0)) {
    ext_oat_present = true;
    ext_oat_sensors.setWaitForConversion(false);
    ext_oat_sensors.requestTemperaturesByAddress(ext_oat_address);
  }
}

#ifdef HAVE_DS2782
void configureBatteryManagement() {
  uint16_t bmp_version = bm.readUserEEPROM_uint16(BMP_VERSION_USER_EEPROM_OFFSET);

  if (bmp_version != BMP_VERSION) {
    if (bmp_version == 0) {
      // The RSGAIN may have been manually tuned, so only set it if the EEPROM was blank to start with.
      bm.writeRSGAIN(BMP_RSGAIN);
    }
    bm.writeRSNSP(BMP_RSNSP);
    bm.writeVCHG(BMP_VCHG);
    bm.writeIMIN(BMP_IMIN);
    bm.writeVAE(BMP_VAE);
    bm.writeIAE(BMP_IAE);
    bm.doCopyData(1);
    delay(10);

    bm.writeUserEEPROM_uint16(BMP_VERSION_USER_EEPROM_OFFSET, BMP_VERSION);
    bm.doCopyData(0);
    delay(10);
  }
}
#endif

void configureRadio() {
  radio.enterCommandMode();

  radio.sendCommand("ATNIAIRBALL_PROBE");
  radio.sendCommand("ATID=5555");
  radio.sendCommand("ATMY=7777");
  radio.sendCommand("ATSM=0");
  radio.sendCommand("ATSP=64");
  radio.sendCommand("ATDL=8888");
  radio.sendCommand("ATAP=1");

  radio.exitCommandMode();
}

void setup() {
  // Ensure the serial command buffer is empty.
  clearCommandBuffer();

  // Initialize the USB serial (for debugging).
  Serial.begin(115200);

#ifdef AIRBALL_DEBUG
  // Wait for a bit for USB serial to come up.
  while(!Serial && millis() < 3000) { }
  Serial.print("Airball Sensor Board v");
  Serial.print(AIRBALL_PROBE_VERSION);
  Serial.print(" alive!");
  Serial.println();
#endif

  // Initialize the XBee serial (for data telemetry).
  Serial1.begin(9600);
  while(!Serial1) {} // Wait for XBee serial to come up.

  // Enable the TCA9548A multiplexer; RST is active-low.
  pinMode(MUX_RESET, OUTPUT);
  digitalWrite(MUX_RESET, HIGH);

  // Initialize the I2C bus at 400 kHz.
  Wire.begin();
  Wire.setClock(400000L);

  // Always enable the TMP102's channel, since its address does not conflict.
  mux.addBaseChannel(MUX_CHANNEL_oat);
  mux.addBaseChannel(MUX_CHANNEL_bm);

  // Prefer pressure in Pascal rather than inH2O.
  dp0.setPressureUnit(AllSensors_DLHR::PressureUnit::PASCAL);
  dpA.setPressureUnit(AllSensors_DLHR::PressureUnit::PASCAL);
  dpB.setPressureUnit(AllSensors_DLHR::PressureUnit::PASCAL);
  baro.setPressureUnit(AllSensors_DLV::PressureUnit::PASCAL);

#ifdef HAVE_TMP275
  oat.setResolution(TI_TMP275::Resolution_12_bits);
#endif

  configureOneWire();

#ifdef HAVE_DS2782
  configureBatteryManagement();
#endif

  configureRadio();

  // Set up a timer to trigger a new measurement.
  Timer1.initialize(MEASUREMENT_INTERVAL_US);
  Timer1.attachInterrupt(callbackReadyForMeasurement);
}

void loop() {
  // Measurement was requested by the global timer; go ahead and start one.
  if (measurement_requested) {
    measurement_requested = false;
    completeMeasurementAndReport();
  }

  // Handle input on the USB-serial as configuration/query commands.
  if (Serial.available()) {
    // Could do better...
    if (command_buf_p >= (command_buf + sizeof(command_buf))) {
      Serial.write("\n\nSerial command buffer overflow. Sorry!\n\n");
      clearCommandBuffer();
    }

    char c = Serial.read();

    Serial.write(c);

    if (c == '\r') {
      Serial.write('\n');
      handleCommand(command_buf);
      clearCommandBuffer();
    }

    if (c != '\r' && c != '\n')
      *command_buf_p++ = c;
  }

  // We're not using the *incoming* data from the XBee serial for anything,
  // but we do need to read it and throw it away or the buffer will fill up.
  radio.discard();
}
