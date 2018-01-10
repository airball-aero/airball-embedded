#include <Wire.h>

#include <TimerOne.h>
#include <TI_TCA9548A.h>
#include <AllSensors_DLHR.h>
#include <SparkFunTMP102.h>

// Which pin is the TCA9548A reset pin tied to?
#define MUX_RESET 4

// The channel configuration for the sensors.
#define MUX_CHANNEL_dp0 0
#define MUX_CHANNEL_dpA 2
#define MUX_CHANNEL_dpB 3
#define MUX_CHANNEL_oat 4

// The I2C address of the TMP102 OAT sensor.
#define I2C_ADDRESS_oat 0x48

// How frequently (in uS) should measurements be taken?
#define MEASUREMENT_INTERVAL_US 50000

// The I2C multiplexer.
TI_TCA9548A mux(&Wire);

// All AllSensors DLHR pressure sensors.
AllSensors_DLHR_L10G_8 dp0(&Wire);
AllSensors_DLHR_L10D_8 dpA(&Wire);
AllSensors_DLHR_L10D_8 dpB(&Wire);

// The OAT sensor.
TMP102 oat(I2C_ADDRESS_oat);

// A global variable and callback function to track whether the timer has fired and it's time
// to collect a new measurement.
volatile boolean measurement_requested = false;
void callbackReadyForMeasurement() {
  measurement_requested = true;
}

void setup() {
  // Initialize the USB serial (for debugging).
  Serial.begin(115200);

  // Initialize the XBee serial (for data telemetry).
  Serial1.begin(9600);

  // Enable the TCA9548A multiplexer; RST is active-low.
  pinMode(MUX_RESET, OUTPUT);
  digitalWrite(MUX_RESET, HIGH);

  // Initialize the I2C bus at 400 kHz.
  Wire.begin();
  Wire.setClock(400000L);

  // Always enable the TMP102's channel, since its address does not conflict.
  mux.addBaseChannel(MUX_CHANNEL_oat);

  // Prefer pressure in Pascal rather than inH2O.
  dp0.setPressureUnit(AllSensors_DLHR::PressureUnit::PASCAL);
  dpA.setPressureUnit(AllSensors_DLHR::PressureUnit::PASCAL);
  dpB.setPressureUnit(AllSensors_DLHR::PressureUnit::PASCAL);

  // Set up a timer to trigger a new measurement.
  Timer1.initialize(MEASUREMENT_INTERVAL_US);
  Timer1.attachInterrupt(callbackReadyForMeasurement);
}

// Append a float value to a string, with an optional (string) delimiter following it.
char float_buf[10];
void appendFloat(char *buf, float value, char const *delim) {
  dtostrf(value, 0, 2, float_buf);
  strcat(buf, float_buf);
  if (delim) strcat(buf, delim);
}

// Construct a telemetry sentence and send it to the provided stream (typically a Serial object).
char sentence[128];
void sendSentence(Stream *stream, float qnh, float oat, float dp0, float dpA, float dpB) {
  // Start with an empty string.
  sentence[0] = 0;

  // Append all variables to the sentence buffer, separated by commas and ended by a CR/NL.
  appendFloat(sentence, qnh, ",");
  appendFloat(sentence, oat, ",");
  appendFloat(sentence, dp0, ",");
  appendFloat(sentence, dpA, ",");
  appendFloat(sentence, dpB, "\r\n");

  // Write the sentence to the stream.
  stream->write(sentence);
}

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
  float oat_temperature = oat.readTempC();

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

  // Send the data sentence to the display.
  sendSentence(&Serial1, 0.0, oat_temperature, dp0.pressure, dpA.pressure, dpB.pressure);
}

void loop() {
  // Measurement was requested by the global timer; go ahead and start one.
  if (measurement_requested) {
    measurement_requested = false;
    completeMeasurementAndReport();
  }
}
