# Programming an Airball Probe

## Add support to the Arduino IDE

Support for the SparkFun Fio v3 must be added to the Arduino IDE, by following the [installation instructions](https://github.com/sparkfun/Arduino_Boards#installation-instructions) to add the SparkFun Arduino Boards repository as an additional board manager URL. Once the SparkFun Arduino Boards are installed, "SparkFun Fio v3" should be available as a board choice (Tools → Board → Arduino AVR Boards → SparkFun Fio v3).

## Install required libraries

The following Arduino libraries must also be installed:

* [TI\_TCA9548A](https://github.com/jeremycole/TI_TCA9548A) – Used for the TI TCA9548A I²C multiplexer.
* [AllSensors\_DLHR](https://github.com/jeremycole/AllSensors_DLHR) – Used for the AllSensors DLHR differential pressure sensors.
* [AllSensors\_DLV](https://github.com/jeremycole/AllSensors_DLV) – Used for the AllSensors DLV absolute pressure sensor.
* [Temperature\_LM75\_Derived](https://github.com/jeremycole/Temperature_LM75_Derived) – Used for the on-board TI TMP275 temperature sensor.
* [OneWire](https://github.com/PaulStoffregen/OneWire) – Used by the DallasTemperature library.
* [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) – Used for the external Maxim DS18B20 1-Wire temperature sensor for measuring outside air temperature.
* [Maxim_DS2782](https://github.com/jeremycole/Maxim_DS2782) – Used for the on-board battery management IC.
* [TimerOne](http://playground.arduino.cc/Code/Timer1) – Used to set up an interrupt to trigger measurement at a specific interval.

The above libraries may be installed using one of the following methods:

1. Using the Arduino IDE's library manager (Sketch → Include Library → Manage Libraries...) to download the most current version and allow automatic updates with new releases.
2. Downloading a specific release (or the "master" branch) from Github as a .ZIP file and installing it using the Arduino IDE's library manager (Sketch → Include Library → Add .ZIP Library...).
3. Manually installing either the .ZIP file as above or cloning the Git repository itself into the `libraries` subdirectory in the Arduino Sketchbook directory (Settings → Sketchbook location) – or doing so here and re-setting the sketchbook directory to this directory.

## Select the correct port

Connect a USB micro-B cable to the Airball Probe and to a computer. The Airball Probe should show up as a USB device in the Arduino IDE (on Mac OS X, it shows up as `/dev/cu.usbmodemXXX` based on which USB port it's plugged into.

Click Tools → Board: ... and select "Sparkfun Fio v3" from the dropdown.

Click Tools → Port: ... and select the appropriate port for your Airball Probe.

## Program the Airball Probe

In order to program the Airball Probe over USB in the Arduino IDE, it must not have a programmer connected to its ICSP port, as this interferes with its bootloader programming its flash. Complete the following steps to program the Airball Probe with a new firmware:

1. **Open** &mdash; Open the `airball_probe/airball_probe.ino` sketch in the Arduino IDE.
2. **Verify/Compile** &mdash; It is useful to ensure that the firmware builds successfully before trying to program the Airball probe with it. Click Sketch → Verify/Compile and ensure that it results in a "Done compiling." message with no errors.
3. **Upload** &mdash; Write the firmware to the Airball Probe. This should take a few seconds and result in a "Done uploading." message, with no errors. Within a few seconds, the probe should come alive again and start sending air data.

# Done!
