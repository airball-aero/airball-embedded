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
#include "probe_calibration.h"
#include "pressures_to_airdata.h"
#include "metric.h"

////////////////////////////////////////////////////////////////////////
//
// Global string buffer for writing sentences

char data_sentence_buffer[1024];

void wifi_send(const char* sentence);

////////////////////////////////////////////////////////////////////////
//
// Metrics

#define METRICS_REPORTING_INTERVAL 100
#define ENABLE_METRICS false

void metrics_begin() {}

Metric metrics_looptime("looptime", METRICS_REPORTING_INTERVAL);
Metric metrics_loopint("loopint", METRICS_REPORTING_INTERVAL);
Metric metrics_dp0_time("dp0_time", METRICS_REPORTING_INTERVAL);
Metric metrics_dpa_time("dpa_time", METRICS_REPORTING_INTERVAL);
Metric metrics_dpb_time("dpb_time", METRICS_REPORTING_INTERVAL);
Metric metrics_baro_time("dpb_time", METRICS_REPORTING_INTERVAL);
Metric metrics_t_time("t_time", METRICS_REPORTING_INTERVAL);
Metric metrics_battery_time("battery_time", 10);
Metric metrics_wifi_time("wifi_time", METRICS_REPORTING_INTERVAL);

void metrics_send_one(Metric& metric) {
  if (metric.ready()) {
    sprintf(data_sentence_buffer, "$M,%s", metric.str());
    wifi_send(data_sentence_buffer);
  }
}

void metrics_send() {
  if (!ENABLE_METRICS) {
    return;
  }
  metrics_send_one(metrics_looptime);
  metrics_send_one(metrics_loopint);
  metrics_send_one(metrics_dp0_time);
  metrics_send_one(metrics_dpa_time);
  metrics_send_one(metrics_dpb_time);
  metrics_send_one(metrics_t_time);
  metrics_send_one(metrics_battery_time);
  metrics_send_one(metrics_wifi_time);
}

////////////////////////////////////////////////////////////////////////
//
// WiFi base station with sensor readings sent over TCP

#define WIFI_SSID "AirballProbe_00000001"
#define WIFI_UDP_PORT 30123

WiFiUDP wifi_udp;
IPAddress wifi_broadcast_ip;

void wifi_begin() {
  WiFi.softAP(WIFI_SSID, "");
  wifi_broadcast_ip = WiFi.broadcastIP();
}

void wifi_send(const char* sentence) {
  metrics_wifi_time.mark();
  wifi_udp.beginPacket(wifi_broadcast_ip, WIFI_UDP_PORT);
  wifi_udp.write((const uint8_t*) sentence, strlen(sentence));
  wifi_udp.endPacket();
  metrics_wifi_time.record();
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

  metrics_dp0_time.mark();
  p.dp0 = pressure_read_one(&pressure_dp0);
  metrics_dp0_time.record();

  metrics_dpa_time.mark();
  p.dpa = pressure_read_one(&pressure_dpa);
  metrics_dpa_time.record();

  metrics_dpb_time.mark();
  p.dpb = pressure_read_one(&pressure_dpb);
  metrics_dpb_time.record();
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

  return p;
}

////////////////////////////////////////////////////////////////////////
//
// Bosch BMP388 barometric pressure sensor readings

// Initialize our sensor
Adafruit_BMP3XX barometer;

void barometer_begin() {
  // Initialize the barometer
  barometer.begin_I2C(0x76, &Wire);
  barometer.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  barometer.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  barometer.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
}

float barometer_read() {
  metrics_baro_time.mark();
  bool success = barometer.performReading();
  metrics_baro_time.record();
  return success ? barometer.pressure : 0.0f;
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
  metrics_t_time.mark();
  float t = thermometer.readTempC();
  metrics_t_time.record();
  return t;
}

////////////////////////////////////////////////////////////////////////
//
// Top level airdata function

#define ENABLE_RAW_AIRDATA false

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
    pressures_to_airdata(&probe_alpha,
			 &probe_beta,
			 &probe_q_over_dp0,
			 p.dp0,
			 p.dpa,
			 p.dpb,
			 &err);
  if (err != 0) {
    memset(&airdata, 0, sizeof(airdata_triple));
  }

  if (ENABLE_RAW_AIRDATA) {
    sprintf(data_sentence_buffer,
	    "$A,%ld,%10.6f,%10.6f,%10.6f,%10.6f,%10.6f",
	    airdata_count,
	    baro,
	    temp,
	    p.dp0,
	    p.dpa,
	    p.dpb);
    wifi_send(data_sentence_buffer);
  }

  if (airdata.q < 0) {
    airdata.q = 0;
  }

  sprintf(data_sentence_buffer,
	  "$AR,%ld,%10.6f,%10.6f,%10.6f,%10.6f,%10.6f",
	  airdata_count,
	  airdata.alpha,   // alpha
	  airdata.beta,    // beta
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

#define ENABLE_BATTERY_MESSAGES false

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

  if (ENABLE_BATTERY_MESSAGES) {
    metrics_battery_time.mark();
    sprintf(data_sentence_buffer,
	    "$B,%ld,%10.6f,%10.6f,%10.6f,%10.6f",
	    battery_measurement_count,
	    (float) battery_sensor.voltage(),
	    (float) battery_sensor.current(),
	    (float) battery_sensor.capacity(),
	    ((float) battery_sensor.capacity()) / ((float) BATTERY_CAPACITY_MAH));
    metrics_battery_time.record();
  }

  wifi_send(data_sentence_buffer);
}

////////////////////////////////////////////////////////////////////////
//
// Central measurement function

void measurements_begin() {
  metrics_begin();
  wifi_begin();
  airdata_begin();
  battery_begin();
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

#define MEASUREMENT_INTERVAL_US 100000 // 100 ms = 10 Hz

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

  Serial.println("setup() end");
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
    metrics_looptime.mark();

    metrics_loopint.record();
    metrics_loopint.mark();

    measurements_read_and_send();

    metrics_looptime.record();

    metrics_send();
  }
}
