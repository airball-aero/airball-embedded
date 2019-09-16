#include <TI_TCA9548A.h>
#include <TimerOne.h>
#include <DallasTemperature.h>
#include "Adafruit_BMP3XX.h"
#include "XBee.h"

// TI TCA9548A I2C multiplexer
TI_TCA9548A mux(&Wire);

// Bosch BMP388 barometric pressure sensor
Adafruit_BMP3XX barometer;

// XBee wireless radio
XBee radio(&Serial1);

// One-Wire bus pin assignment
#define ONE_WIRE_BUS_PIN_NUMBER 4

// Dallas Semiconductor temperature probe
OneWire onewire_bus(ONE_WIRE_BUS_PIN_NUMBER);
DallasTemperature ext_oat_sensors(&onewire_bus);
DeviceAddress ext_oat_address;

// Which pin is the TCA9548A reset pin tied to?
#define MUX_RESET 4

// I2C mux channels for the various sensors
#define MUX_CHANNEL_DP0       0
#define MUX_CHANNEL_DPA       1
#define MUX_CHANNEL_DPB       2
#define MUX_CHANNEL_BAROMETER 3
#define MUX_CHANNEL_BATTERY   4

// Standard I2C address of the Honeywell HSC sensors
#define HSC_I2C_ADDRESS 0x28

// The full scale of the Honeywell HSC sensors, in Pascals
#define HSC_FULL_SCALE 4000

// How frequently (in uS) should measurements be taken?
// #define MEASUREMENT_INTERVAL_US 50000
#define MEASUREMENT_INTERVAL_US 500000

// Speed of the I2C bus
#define I2C_BUS_SPEED 400000L // 400 kHz

////////////////////////////////////////////////////////////////////////
//
// Wireless packets
//

void configureRadio() {
  radio.enterCommandMode();

  radio.sendCommand("ATNIAIRBALL_PROBE");
  radio.sendCommand("ATSM=0");
  radio.sendCommand("ATSP=64");
  radio.sendCommand("ATDL=8888"); // Default destination address
  radio.sendCommand("ATID=5555"); // Network ID
  radio.sendCommand("ATMY=7777"); // My ID
  radio.sendCommand("ATAP=1");    // API mode

  radio.exitCommandMode();
}

void send_packet(char* buf, uint16_t len) {
  radio.send_packet_x01_send_16_bit(
      0x8888,
      buf,
      len);
}

////////////////////////////////////////////////////////////////////////
//
// Primary air data
//

uint16_t read_hsc_raw(uint16_t mux_channel, uint16_t sensor_address) {
  uint16_t p = 0;
  uint16_t t = 0;
  bool found = false;

  mux.selectChannel(mux_channel);

  while (!found) {

    Wire.requestFrom(sensor_address, 4);
    
    uint8_t b[4];
  
    for (int i = 0; i < 4; i++) {
      b[i] = Wire.read();
    }
  
    uint8_t status[2];
    status[0] = b[0] & (0x01 << 6);
    status[1] = b[0] & (0x01 << 7);

    if (status[0] == 0 && status[1] == 0) {
      p |= b[1];
      p |= ((b[0] & 0x3f) << 8);
  
      t |= ((b[3] & 0xe0) >> 5);
      t |= (b[2] << 8);

      found = true;
    }

    Wire.endTransmission();
  }

  return p;
}

float pressure_count_to_value_hsc(uint16_t raw, float full_scale) {
  float p_min = -full_scale;
  float p_max = full_scale;
  float n_ratio = ((float) raw) / ((float) pow(2, 14));
  return p_min + 1.25 * (n_ratio - 0.1) * (p_max - p_min);
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

char sentence[128];

void send_air_data_sentence(
    unsigned long seq,
    float baro,
    float oat,
    float dp0,
    float dpA,
    float dpB) {
  // Start with an empty string.
  sentence[0] = 0;

  // Append all variables to the sentence buffer, separated
  // by commas and ended by a CR/NL.
  appendString(sentence, "$A", ",");
  appendInteger(sentence, seq, ",");
  appendFloat(sentence, baro, ",");
  appendFloat(sentence, oat, ",");
  appendFloat(sentence, dp0, ",");
  appendFloat(sentence, dpA, ",");
  appendFloat(sentence, dpB, "\r\n");

  // Write the sentence to the stream.
  send_packet(sentence, strlen(sentence));
}

unsigned long airdata_seq = 0;

void read_airdata() {
  float baro = 0.0;
  mux.selectChannel(MUX_CHANNEL_BAROMETER);
  if (barometer.performReading()) {
    baro = barometer.pressure;
  }

  float oat = ext_oat_sensors.getTempC(ext_oat_address);
  ext_oat_sensors.requestTemperaturesByAddress(ext_oat_address);

  float dp0 = pressure_count_to_value_hsc(
      read_hsc_raw(MUX_CHANNEL_DP0, HSC_I2C_ADDRESS),
      HSC_FULL_SCALE);
  float dpA = pressure_count_to_value_hsc(
      read_hsc_raw(MUX_CHANNEL_DPA, HSC_I2C_ADDRESS),
      HSC_FULL_SCALE);
  float dpB = pressure_count_to_value_hsc(
      read_hsc_raw(MUX_CHANNEL_DPB, HSC_I2C_ADDRESS),
      HSC_FULL_SCALE);

  send_air_data_sentence(
    airdata_seq++,
    baro,
    oat,
    dp0,
    dpA,
    dpB);
}

////////////////////////////////////////////////////////////////////////
//
// Central measurement function
//

void completeMeasurementAndReport() {
  read_airdata();
}

////////////////////////////////////////////////////////////////////////
//
// Arduino entry points
//

// A global variable and callback function to track whether the timer
// has fired and it's time to collect a new measurement.
volatile boolean measurement_requested = false;

void callbackReadyForMeasurement() {
  measurement_requested = true;
}

void setup() {

  Serial.begin(19200);
  while (!Serial) {}
  
  // Initialize the XBee serial (for data telemetry).
  Serial1.begin(19200);
  while(!Serial1) {}
  configureRadio();
  
  // Tie the I2C mux reset pin high and keep it there
  pinMode(MUX_RESET, OUTPUT);
  digitalWrite(MUX_RESET, HIGH);

  // Initialize the I2C bus
  Wire.begin();
  Wire.setClock(I2C_BUS_SPEED);

  // Initialize the barometer
  mux.selectChannel(MUX_CHANNEL_BAROMETER);
  barometer.begin();
  barometer.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  barometer.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  barometer.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);

  // Initialize the 1Wire temperature sensor
  ext_oat_sensors.begin();
  if (ext_oat_sensors.getAddress(ext_oat_address, 0)) {
    ext_oat_sensors.setWaitForConversion(false);
    ext_oat_sensors.requestTemperaturesByAddress(ext_oat_address);
  }

  // Set up a timer to trigger a new measurement.
  Timer1.initialize(MEASUREMENT_INTERVAL_US);
  Timer1.attachInterrupt(callbackReadyForMeasurement);
}

void loop() {
  // Measurement was requested by the global timer; go ahead and start one.
  if (measurement_requested) {
    measurement_requested = false;

    unsigned long t = millis();
    completeMeasurementAndReport();
    Serial.println(millis() - t);
    
    // We're not using the *incoming* data from the XBee serial for anything,
    // but we do need to read it and throw it away or the buffer will fill up.
    radio.discard();
  }
}
