#include <TI_TCA9548A.h>
#include <TimerOne.h>
#include <OneWire.h>
#include <SparkFunBQ27441.h>
#include <Adafruit_BMP3XX.h>

#include "XBee.h"

// TI TCA9548A I2C multiplexer
TI_TCA9548A mux(&Wire);

// Bosch BMP388 barometric pressure sensor
Adafruit_BMP3XX barometer;

// BQ27441 battery monitor
BQ27441 battery;

// XBee wireless radio
XBee radio(&Serial1);

// One-Wire bus pin assignment
#define ONE_WIRE_BUS_PIN_NUMBER 4

// Which pin is the TCA9548A reset pin tied to?
#define MUX_RESET 4

// Whether to perform autozero
#define PERFORM_AUTOZERO true

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
#define MEASUREMENT_INTERVAL_US 50000 // 50 ms = 20 Hz

// How many airdata samples per OAT sample?
#define OAT_MEASUREMENT_INTERVAL 50

// How many airdata samples per battery sample?
#define BATTERY_MEASUREMENT_INTERVAL 20

// How many airdata samples for autozero?
#define AUTOZERO_NUM_READINGS 100

// Speed of the I2C bus
#define I2C_BUS_SPEED 400000L // 400 kHz

// Battery capacity in mAH
#define BATTERY_CAPACITY_MAH 1000

// LED for signaling data out
#define RXLED 17

// Outgoing sentence, for global use
char sentence[128];

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
  radio.sendCommand("ATAP=0");    // API mode

  radio.exitCommandMode();
}

void send_packet(char* buf, uint16_t len) {
  buf[len] = '\0';
  radio.write(buf);
}

////////////////////////////////////////////////////////////////////////
//
// Temperature
//

#define READSCRATCH    0xBE
#define STARTCONVO     0x44

OneWire wire(ONE_WIRE_BUS_PIN_NUMBER);
uint8_t oat_address[8];

bool valid_device_address(uint8_t *addr) {
  return (wire.crc8(addr, 7) == addr[7]);
}

void init_oat() {
  bool found = false;
  wire.reset_search();
  while (wire.search(oat_address)) {
    if (valid_device_address(oat_address)) {
      found = true;
    }
  }
  start_oat_conversion();
}

float start_oat_conversion() {
  wire.skip();
  wire.write(STARTCONVO, false);
  wire.reset();  
}

float read_oat() {
  wire.select(oat_address);
  wire.write(READSCRATCH);
  uint8_t temp_lsb = wire.read();
  uint8_t temp_msb = wire.read();
  wire.reset();

  int16_t raw = (((int16_t)temp_msb) << 8) | temp_lsb;
  return (float)(raw >> 3) * 0.5;
}

float measure_oat() {
  float t = read_oat();
  start_oat_conversion();
  return t;
}

////////////////////////////////////////////////////////////////////////
//
// Air data
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

void send_air_data_sentence(
    unsigned long seq,
    float baro,
    float oat,
    float dp0,
    float dpA,
    float dpB) {
  sentence[0] = 0;

  appendString(sentence, "$A", ",");
  appendInteger(sentence, seq, ",");
  appendFloat(sentence, baro, ",");
  appendFloat(sentence, oat, ",");
  appendFloat(sentence, dp0, ",");
  appendFloat(sentence, dpA, ",");
  appendFloat(sentence, dpB, "\r\n");

  send_packet(sentence, strlen(sentence));
}

unsigned long airdata_seq = 0;

unsigned long oat_measurement_count = 0;
float oat;

unsigned long autozero_count = 0;
bool autozero_complete = !PERFORM_AUTOZERO;

float offset_dp0 = 0;
float offset_dpA = 0;
float offset_dpB = 0;

void read_airdata() {
  float baro = 0.0;
  mux.selectChannel(MUX_CHANNEL_BAROMETER);
  if (barometer.performReading()) {
    baro = barometer.pressure;
  }

  if (oat_measurement_count++ > OAT_MEASUREMENT_INTERVAL) {
    oat = measure_oat();
    oat_measurement_count = 0;
  }

  float dp0 = pressure_count_to_value_hsc(
      read_hsc_raw(MUX_CHANNEL_DP0, HSC_I2C_ADDRESS),
      HSC_FULL_SCALE);
  float dpA = pressure_count_to_value_hsc(
      read_hsc_raw(MUX_CHANNEL_DPA, HSC_I2C_ADDRESS),
      HSC_FULL_SCALE);
  float dpB = pressure_count_to_value_hsc(
      read_hsc_raw(MUX_CHANNEL_DPB, HSC_I2C_ADDRESS),
      HSC_FULL_SCALE);

  if (autozero_complete) {
    dp0 -= offset_dp0;
    dpA -= offset_dpA;
    dpB -= offset_dpB;
  } else {
    if (autozero_count == AUTOZERO_NUM_READINGS) {
      offset_dp0 /= (float) AUTOZERO_NUM_READINGS;
      offset_dpA /= (float) AUTOZERO_NUM_READINGS;
      offset_dpB /= (float) AUTOZERO_NUM_READINGS;
      autozero_complete = true;
    } else {
      offset_dp0 += dp0;
      offset_dpA += dpA;
      offset_dpB += dpB;
    }
    autozero_count++;
  }

  // dp0 should physically never be negative, so we clamp it
  if (dp0 < 0) { dp0 = 0; }

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
// Battery
//

void send_battery_sentence(
  unsigned long seq, 
  float bm_V, 
  float bm_mA, 
  float bm_capacity_mAh, 
  float bm_capacity_pct) {
  // Start with an empty string.
  sentence[0] = 0;

  // Append all variables to the sentence buffer, separated by commas and ended by a CR/NL.
  appendString(sentence, "$B", ",");
  appendInteger(sentence, seq, ",");
  appendFloat(sentence, bm_V, ",");
  appendFloat(sentence, bm_mA, ",");
  appendFloat(sentence, bm_capacity_mAh, ",");
  appendFloat(sentence, bm_capacity_pct, "\r\n");

  send_packet(sentence, strlen(sentence));
}

unsigned long battery_seq = 0;

unsigned long battery_measurement_count = 0;

void read_battery() {
  if (battery_measurement_count++ > BATTERY_MEASUREMENT_INTERVAL) {
    mux.selectChannel(MUX_CHANNEL_BATTERY);
    send_battery_sentence(
      battery_seq++,
      (float) battery.voltage(),
      (float) battery.current(),
      (float) battery.capacity(),
      ((float) battery.capacity()) / ((float) BATTERY_CAPACITY_MAH));
    battery_measurement_count = 0;
  }
}

////////////////////////////////////////////////////////////////////////
//
// Central measurement function
//

void completeMeasurementAndReport() {
  read_airdata();
  read_battery();
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
  // Initialize the LED for signaling the event loop
  pinMode(RXLED, OUTPUT);
  
  // Initialize the XBee serial (for data telemetry).
  Serial1.begin(19200);
  while (!Serial1) {}
  configureRadio();

  // Tie the I2C mux reset pin high and keep it there
  pinMode(MUX_RESET, OUTPUT);
  digitalWrite(MUX_RESET, HIGH);

  // Initialize the I2C bus
  Wire.begin();
  Wire.setClock(I2C_BUS_SPEED);

  // Initialize the barometer
  mux.selectChannel(MUX_CHANNEL_BAROMETER);
  barometer.begin_I2C();
  barometer.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  barometer.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  barometer.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);

  // Initialize the battery monitoring
  mux.selectChannel(MUX_CHANNEL_BATTERY);
  battery.begin();
  battery.setCapacity(BATTERY_CAPACITY_MAH); 

  // Initialize the 1Wire temperature sensor
  init_oat();

  // Set up a timer to trigger a new measurement.
  Timer1.initialize(MEASUREMENT_INTERVAL_US);
  Timer1.attachInterrupt(callbackReadyForMeasurement);
}

bool led_state = false;

void loop() {
  // Measurement was requested by the global timer; go ahead and start one.
  if (measurement_requested) {
    measurement_requested = false;

    completeMeasurementAndReport();
    
    // We're not using the *incoming* data from the XBee serial for anything,
    // but we do need to read it and throw it away or the buffer will fill up.
    radio.discard();

    digitalWrite(RXLED, led_state ? HIGH : LOW);
    led_state = !led_state;
  }
}
