# Airball Sensor Board V8 design

## Quick start: Eagle files and routing considerations

Files for the *main* board are `airball_sensor_board_v8_main.*` in this directory.

The *temperature sensor* daughter board will be in files named `airball_sensor_board_v8_temp.*` and is a simple 4-connector I2C sensor breakout. As of this writing, it is not yet designed. For now, we can prove out our design using a Sparkfun breakout board.

We have used whatever libraries we can -- primarily from Sparkfun, but also others. The main issue we have is that the [BMP388 sensor footprint](https://www.snapeda.com/parts/BMP388/Bosch%20Sensortec/view-part/) we used generates some "clearance" errors, and it's not clear whether that's a problem with our design rules or with the footprint. **UPDATE: Changing design rules fixes that, so not an issue.**

The design as shown includes enough vias to minimally connect the ground planes and avoid airwires, but does *not* include extra vias to ensure good grounding throughout. We will add this after we have checked out the design some more.

## Introduction for electronic designers

Airball is a hobbyist-grade prototype airplane instrument. It comes in two parts: (a) a probe, which sits outside the airplane and measures information about the airflow; and (b) a display, which is typically a Raspberry Pi with some screen and which shows a visualization. The sensor board is the heart of the probe.

The entire probe should physically look something like what is depicted in [the following series of images](https://photos.app.goo.gl/cj4wwVtSAWHoV7az6). We have made several prototypes with "breakout boards", such as [this one](http://www.airball.aero/2019/10/new-probe-design.html). Our goal now is to make something integrated, reliable, and ergonomic that can be sent out to people to build as part of a kit.

The probe measures pressures impinging on holes in the surface of some 3D printed parts, and uses these to infer data about the airflow. It also measures barometric pressure, and the temperature of the air. It does some really simple math, and then sends out the data over WiFi.

The design requirements of the probe, and by corollary the sensor board, are:

* Can be made from scratch from Open Source designs using 3D printing and hobbyist-accessible electronics fabrication.
* Can be inexpensively built into a kit. The kit should require zero soldering on the part of the kit builder.
* Battery powered and rechargeable.
* Can be programmed by a hobbyist using an Arduino IDE; it should show up as some kind of "Arduino dev board" that just happens to have some extra sensors attached to it.

This has led us to impose the following specific design constraints on the board:

* Mostly single sided PCB for inexpensive and easy design.
* Components that are not simple, single-sided SMT parts can be easily added on in a "post manufacture" step in a workshop.
* Battery charging circuit built in.
* USB interface circuit to the microprocessor built in.

## Overall design

The following are high-level design decisions we have made. The circuit has been prototyped and shown to work as required in [this blog post](http://www.airball.aero/2020/03/esp32-probe-ongoing-programming.html). The picture in the posting shows the breakout boards and dev boards required to prototype the system. The design we are creating right now merely integrates these components onto one compact SMT board that does not have the soldering and wiring, which in our experience tends to loosen when put into the high vibration environment of an airplane.

### Microcontroller

We are using an [ESP32-WROOM-32U](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf) microcontroller. We are specifically choosing the model with the u.FL antenna connector so that we have flexibility to use whatever sort of antenna we want.

### USB interface to microcontroller

We are copying the [ESP32-DevKitC](https://www.espressif.com/en/products/hardware/esp32-devkitc/overview) circuitry for USB. In so doing, we are using a [SiLabs CP2102](https://www.silabs.com/documents/public/data-sheets/CP2102-9.pdf) USB-to-serial chip, and a small transistor network.

### Power supply

We are using a high capacity (3.2 Ah or more) 18650 Li-Ion battery, having verified that it gives us the necessary battery life (more than 12 hours, or "all day", with very conservative use).

Our battery charging circuitry is a copy of the [Sparkfun Battery Babysitter](https://www.sparkfun.com/products/13777). Our battery charger copies the Sparkfun product and is a [TI BQ24075](http://www.ti.com/lit/ds/slusau3b/slusau3b.pdf). Also as with the Sparkfun product, we use [TI BQ27441-G1](https://www.ti.com/lit/ds/symlink/bq27441-g1.pdf) I2C battery fuel gauge. We connect this to the microcontroller's I2C bus so we can send battery life information over WiFi.

### Pressure sensors

We need to measure three differential pressures. For this, we have chosen [Honeywell TruStability HSC Series](https://sensing.honeywell.com/honeywell-sensing-trustability-hsc-series-high-accuracy-board-mount-pressure-sensors-50099148-a-en.pdf) sensors. The electronically important aspects of these are that the ones we chose operate at 3.3V and have a SPI interface. Since they do not accept any data from the SPI bus master, they do not have a MOSI pin -- only MISO and SCLK. These chips have connectors for small hoses, which we will use to connect them to the proper parts of the outside of the probe.

### Barometer

To measure barometric pressure, we use a [Bosch BMP388](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/pressure-sensors-bmp388.html) chip mounted to the board. In order to feed it the exact correct pressure, we will create a 3D printed "cap" on top of it, screwed to the PCB, that will attach to a hose that goes to the correct location on the outside of the probe. We connect to the barometer using I2C.

### Thermometer

To measure air temperature, we use a [TI TMP102](https://www.ti.com/lit/ds/symlink/tmp102.pdf) thermometer mounted on a daughterboard that sticks it outside in the airflow. Our main board contains a series of I2C pinouts on an 0.1" header to facilitate this.

### User interface

We provide a USB-C port for charging and programming, and an ON/OFF switch following the recommendations of the battery charger chip's data sheet.

We provide 4 LEDs, corresponding to the following signals:

* USB input voltage available
* Battery charging
* System power on
* Microcontroller activity (one LED available for use by programs)

## Design notes

### Part selection

**Unless specified**, we standardize on the following to minimize unique parts count:

* 0805 10V capacitors
* 0603 1% resistors
* [0603 2 mA 1.8V LEDs](https://www.mouser.com/Optoelectronics/LED-Lighting/LED-Emitters/Standard-LEDs-SMD/_/N-b1bb1?P=1yzvlkxZ1yopobeZ1yuo9ge)

## Power and battery charging circuit

The battery  charger is a TI [BQ24075](http://www.ti.com/lit/ds/slusau3b/slusau3b.pdf). We are using the BQ24075 (as opposed to its sibling devices) because (a) it is what's used on the Sparkfun product; and (b) so that we can have the SYSOFF feature which gives us a convenient on/off switch.

The SparkFun design uses one 1206 50V cap to decouple the USB input voltage, while other caps are all 0805 10V, or one 0603 with unspecified voltage limits.

We follow Figure 41 in the data sheet to construct a "standalone battery charger".  We choose:

* IN decoupling cap = 1 μF **1206 50V**.
* OUT and BAT decoupling caps 10  μF.
* !PGOOD and !CHG have LEDs via 1.5 kΩ resistors to charger OUT.
* !CE pulled directly to GND.
* EN2 via 100 kΩ resistor to OUT, EN1 to GND. This enables ILIM.
* ILIM programmed to 1 A. RLIM = 1550 AΩ / 1 A = 1.55 kΩ. Choose closest value of 1.54 kΩ.
* We are using at least a 2.6 Ah 18650 battery. This means our maximum safe charging rate should be about 1.3 A. Let's choose 1A for even greater safety. R_ISET_ = 890 AΩ / 1A, which means R_ISET_  should be at least 890 Ω, and the next size up is 898 Ω.
* TMR is disabled by attaching to GND.
* No temperature sensing. TS is a 10 kΩ to GND.
* SYSOFF uses a 100 kΩ resistor to pull it up to VBATT. This is copied from the SparkFun product.

The charger's OUT is sent to an LT1963 3.3 V regulator. The output of the regulator is the system's VCC, and it is decoupled at the regulator with an additional 10 μF cap.

We could pull the "power ON" signal for an LED from the charger's OUT, or from VCC. We prefer VCC since it has a more predictable voltage. Given the formula, R = (VCC - Vf) / If, we get R = (3.3 V - 1.8 V) / 2 mA = 750 Ω. This is a standard resistance size.

This completes the charging circuit _per se_. We also want to monitor the charging state. Similarly to the Sparkfun product, we choose the TI [BQ27441-G1](https://www.ti.com/lit/ds/symlink/bq27441-g1.pdf) I2C battery fuel gauge. This is wired per Figure 6 in the data sheet:

* Current sense resistor, 0.01 Ω **1206**, and since the resistance is super low, we don't need to worry about the power rating.
* Additional 1 μF cap on the BAT input.
* Decouple the VDD output with an 0.47 μF cap.
* Per data sheet, GPOUT to VCC and BIN to VSS via 10 kΩ resistors.
* SDA and SCL obviously connected to the system's I2C bus.

This completes the battery and power circuitry. If we include the SYSOFF switch in all that "module", then the connections to the remainder of the circuit are only:

* Power: GND, VCC
* I2C: SDA, SCL

### USB interface

The USB interface and transistors are designed as a copy of the [ESP32-DevKitC](https://www.espressif.com/en/products/hardware/esp32-devkitc/overview). We duplicate the transistor network from the ESP32-DevKitC that allows a programmer on the USB side to reset the microcontroller by manipulating the DTR and RTS lines. We have added an ESD protection diode array per the recommendations of the TI CP2102 data sheet.

### Microcontroller

The microcontroller installation is straightforward. We have added decoupling caps for VCC following the recommendations of the data sheet.

### Pressure sensors

The Honeywell pressure sensors are mounted in straighforward fashion.

### Barometer

The barometer is straightforward, and connects to the system's I2C bus.

### LEDs

Where appropriate, we have followed the data sheet recommendations for how to wire LEDs to the charger system. For the remaining two, we have:

* One LED from VCC go GND, indicating system power on.
* One LED from a microcontroller's GPIO to GND, for use by programs.

In both of these cases we have computed a series resistance of 750 Ω for each LED, given the specs of the LEDs we are using and assuming that the input in both cases is exactly 3.3 V.