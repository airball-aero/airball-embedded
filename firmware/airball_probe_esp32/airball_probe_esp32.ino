#include <Wire.h>

#include <HoneywellTruStabilitySPI.h>
#include <Adafruit_BMP3XX.h>
#include <SparkFunTMP102.h>
#include <SparkFunBQ27441.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <vector>

#include "calibration_surface.h"
#include "v2_probe_calibration.h"
#include "pressures_to_airdata.h"
#include "metric.h"

////////////////////////////////////////////////////////////////////////
//
// Global string buffer for writing sentences

char data_sentence_buffer[1024];

////////////////////////////////////////////////////////////////////////
//
// WiFi base station with sensor readings sent over TCP

#define WIFI_SSID "ProbeOnAStick"
#define WIFI_PORT 80

WiFiServer wifi_server(WIFI_PORT);
std::vector<WiFiClient> wifi_clients;

void wifi_begin() {
  WiFi.softAP(WIFI_SSID, "");
  wifi_server.begin();
}

void wifi_send(const char* sentence) {
  while (true) {
    WiFiClient client = wifi_server.available();
    if (client) {
      wifi_clients.push_back(client);
    } else {
      break;
    }
  }

  for (auto it = wifi_clients.begin() ; it != wifi_clients.end(); ) {
    if (it->connected()) {
      it->printf("%s\n", sentence);
      ++it;
    } else {
      wifi_clients.erase(it);
    }
  }
}

////////////////////////////////////////////////////////////////////////
//
// Honeywell TruStability pressure sensor readings

// SPI chip select GPIOs for the three pressure sensors.
#define PRESSURE_SELECT_DP0 25
#define PRESSURE_SELECT_DPA 26
#define PRESSURE_SELECT_DPB 27

// Range of the sensors we are using, in Pascals
#define PRESSURE_SENSOR_RANGE 6000

// Whether to perform an autozero of the pressure sensors
#define PRESSURE_PERFORM_AUTOZERO true

// How many airdata samples for autozero?
#define PRESSURE_AUTOZERO_NUM_READINGS 100

// Initialize our three sensors 
TruStabilityPressureSensor pressure_dp0(PRESSURE_SELECT_DP0,
					-PRESSURE_SENSOR_RANGE,
					 PRESSURE_SENSOR_RANGE);
TruStabilityPressureSensor pressure_dpa(PRESSURE_SELECT_DPA,
					-PRESSURE_SENSOR_RANGE,
					 PRESSURE_SENSOR_RANGE);
TruStabilityPressureSensor pressure_dpb(PRESSURE_SELECT_DPB,
					-PRESSURE_SENSOR_RANGE,
					 PRESSURE_SENSOR_RANGE);

float pressure_read_one(TruStabilityPressureSensor* sensor) {
  if (sensor->readSensor() == 0 ) {
    return sensor->pressure();
  }
  return 0.0f;
}

typedef struct pressures_struct {
  float dp0;
  float dpa;
  float dpb;
} pressures;

unsigned long pressure_autozero_count = 0;
bool pressure_autozero_complete = !PRESSURE_PERFORM_AUTOZERO;
pressures pressure_autozero_offset = {
  0.0f,
  0.0f,
  0.0f,
};

void pressures_begin() {
  // Initialize the pressure sensors
  pressure_dp0.begin();
  pressure_dpa.begin();
  pressure_dpb.begin();
}

struct pressures_struct pressures_read() {
  pressures p;
  
  p.dp0 = pressure_read_one(&pressure_dp0);
  p.dpa = pressure_read_one(&pressure_dpa);
  p.dpb = pressure_read_one(&pressure_dpb);

  if (pressure_autozero_complete) {
    p.dp0 -= pressure_autozero_offset.dp0;
    p.dpa -= pressure_autozero_offset.dpa;
    p.dpb -= pressure_autozero_offset.dpb;
  } else {
    if (pressure_autozero_count == PRESSURE_AUTOZERO_NUM_READINGS) {
      pressure_autozero_offset.dp0 /= (float) PRESSURE_AUTOZERO_NUM_READINGS;
      pressure_autozero_offset.dpa /= (float) PRESSURE_AUTOZERO_NUM_READINGS;
      pressure_autozero_offset.dpb /= (float) PRESSURE_AUTOZERO_NUM_READINGS;
      pressure_autozero_complete = true;
    } else {
      pressure_autozero_offset.dp0 += p.dp0;
      pressure_autozero_offset.dpa += p.dpa;
      pressure_autozero_offset.dpb += p.dpb;
    }
    pressure_autozero_count++;
  }

  // dp0 should physically never be negative, so we clamp it
  if (p.dp0 < 0) { p.dp0 = 0; }
  
  return p;
}

////////////////////////////////////////////////////////////////////////
//
// Bosch BMP388 barometric pressure sensor readings

// Initialize our sensor
Adafruit_BMP3XX barometer;

void barometer_begin() {
  // Initialize the barometer
  barometer.begin(0x76);
  barometer.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  barometer.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  barometer.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
}

float barometer_read() {
  if (barometer.performReading()) {
    return barometer.pressure;
  }
  return 0.0f;
}

////////////////////////////////////////////////////////////////////////
//
// TI TMP102 temperature readings

// Initialize our sensor
TMP102 thermometer;

void thermometer_begin() {
  thermometer.begin();
  thermometer.wakeup();
}

float thermometer_read() {
  return thermometer.readTempC();
}

////////////////////////////////////////////////////////////////////////
//
// Top level airdata function

void airdata_begin() {
  pressures_begin();
  barometer_begin();
  thermometer_begin();
}

long airdata_count = 0L;

void airdata_read_and_send() {
  pressures p = pressures_read();
  float baro = barometer_read();
  float temp = thermometer_read();
  
  int err = 0;
  airdata_triple airdata =
    pressures_to_airdata(&v2_probe_alpha,
			 &v2_probe_beta,
			 &v2_probe_q_over_dp0,
			 p.dp0,
			 p.dpa,
			 p.dpb,
			 &err);
  if (err != 0) {
    memset(&airdata, 0, sizeof(airdata_triple));
  }

  sprintf(data_sentence_buffer,
	  "$A,%ld,%10.6f,%10.6f,%10.6f,%10.6f,%10.6f",
	  airdata_count,
	  baro,
	  temp,
	  p.dp0,
	  p.dpa,
	  p.dpb);
  wifi_send(data_sentence_buffer);
  
  sprintf(data_sentence_buffer,
	  "$AR,%ld,%10.6f,%10.6f,%10.6f,%10.6f,%10.6f",
	  airdata_count,
	  -airdata.alpha,  // alpha
	  -airdata.beta,   // beta
	  airdata.q,       // q
	  baro,            // p TODO(ihab): Correction
	  temp);           // T
  wifi_send(data_sentence_buffer);

  airdata_count++;
}


////////////////////////////////////////////////////////////////////////
//
// BQ27441 battery monitor
//

// Battery capacity
#define BATTERY_CAPACITY_MAH 3400

// How many airdata samples per battery sample?
#define BATTERY_MEASUREMENT_INTERVAL 50

// Initialize our sensor
BQ27441 battery_sensor;

void battery_begin() {
  battery_sensor.begin();
  battery_sensor.setCapacity(BATTERY_CAPACITY_MAH); 
}

long battery_measurement_interval_count = 0L;
long battery_measurement_count = 0L;

void battery_read_and_send() {
  if (battery_measurement_interval_count++ < BATTERY_MEASUREMENT_INTERVAL) {
    return;
  }
  battery_measurement_interval_count = 0;
  battery_measurement_count += 1;
  
  sprintf(data_sentence_buffer,
	  "$B,%ld,%10.6f,%10.6f,%10.6f,%10.6f",
	  battery_measurement_count,
	  (float) battery_sensor.voltage(),
	  (float) battery_sensor.current(),
	  (float) battery_sensor.capacity(),
	  ((float) battery_sensor.capacity()) / ((float) BATTERY_CAPACITY_MAH));
  wifi_send(data_sentence_buffer);
}

////////////////////////////////////////////////////////////////////////
//
// Metrics

#define METRICS_REPORTING_INTERVAL 100
#define ENABLE_METRICS false

void metrics_begin() {}

Metric metrics_looptime;
Metric metrics_loopint;

long metrics_last_loop_start = 0L;
long metrics_this_loop_start = 0L;

long metrics_count = 0L;

void metrics_loop_start() {
  if (!ENABLE_METRICS) {
    return;
  }
  long t = micros();
  if (metrics_last_loop_start == 0L) {
    metrics_last_loop_start = t;
  } else {
    metrics_loopint.add(t - metrics_last_loop_start);
    metrics_last_loop_start = t;
  }
  metrics_this_loop_start = t;
}

void metrics_loop_end() {
  if (!ENABLE_METRICS) {
    return;
  }
  metrics_looptime.add(micros() - metrics_this_loop_start);
  if (++metrics_count > METRICS_REPORTING_INTERVAL) {
    sprintf(data_sentence_buffer, "$M,looptime,%s", metrics_looptime.str());
    wifi_send(data_sentence_buffer);
    sprintf(data_sentence_buffer, "$M,loopint,%s", metrics_loopint.str());
    wifi_send(data_sentence_buffer);
    metrics_count = 0L;
  }
}

////////////////////////////////////////////////////////////////////////
//
// Central measurement function

void measurements_begin() {
  wifi_begin();
  airdata_begin();
  battery_begin();
  metrics_begin();
}

void measurements_read_and_send() {
  airdata_read_and_send();
  battery_read_and_send();
}

////////////////////////////////////////////////////////////////////////
//
// Arduino entry points

hw_timer_t *timer = NULL;
portMUX_TYPE timer_mux = portMUX_INITIALIZER_UNLOCKED;
bool timer_start_measurement = false;

void timer_fired() {
  portENTER_CRITICAL(&timer_mux);
  timer_start_measurement = true;
  portEXIT_CRITICAL(&timer_mux);
}

// Speed of the I2C bus
#define I2C_BUS_SPEED 400000L // 400 kHz

// How frequently (in uS) should measurements be taken?
#define MEASUREMENT_INTERVAL_US 50000 // 50 ms = 20 Hz

void setup() {
  // Initialize the SPI bus
  SPI.begin();
  
  // Initialize the I2C bus
  Wire.begin();
  Wire.setClock(I2C_BUS_SPEED);

  measurements_begin();

  // Create semaphore to inform us when the timer has fired
  // timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual
  // for more info).
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &timer_fired, true);
  timerAlarmWrite(timer, MEASUREMENT_INTERVAL_US, true);
  timerAlarmEnable(timer);
}

void loop() {
  bool measure = false;
  
  portENTER_CRITICAL(&timer_mux);
  if (timer_start_measurement) {
    measure = true;
    timer_start_measurement = false;
  }
  portEXIT_CRITICAL(&timer_mux);
  
  if (measure) {
    metrics_loop_start();
    measurements_read_and_send();
    metrics_loop_end();
  }
}
