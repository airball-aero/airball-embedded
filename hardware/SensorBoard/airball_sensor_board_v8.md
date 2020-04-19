# Airball Sensor Board V8 design notes

## Overall design

The design involves a main board that contains:

1. 18650 battery, battery charger, and I2C battery fuel gauge
1. ESP32-WROOM-32U microcontroller and Wi-Fi module
1. Three Honeywell SPI HSC digital pressure sensors
1. One Bosch BMP388 I2C digital barometer

This connects via a 4 conductor I2C pin header to a daughterboard that contains:

1. TI TMP102 I2C digital thermometer

The battery charging circuitry is based on the [Sparkfun Battery Babysitter](https://www.sparkfun.com/products/13777) product.

## Power and battery charging circuit

The battery  charger is a TI [BQ24075](http://www.ti.com/lit/ds/slusau3b/slusau3b.pdf). We are using the BQ24075 (as opposed to its sibling devices) because (a) it is what's used on the Sparkfun product; and (b) so that we can have the SYSOFF feature which gives us a convenient on/off switch.

The SparkFun design uses one 1206 50V cap to decouple the USB input voltage, while other caps are all 0805 10V, or one 0603 with unspecified voltage limits. Apart from the one input cap, we will standardize on 0805 10V caps for all our work to minimize unique parts count. For the same reason, we will standardize on 0603 resistors with 1% tolerance for all our resistors except where we need something special.

We standardize on [2 mA 1.8V 0603 LEDs](https://www.mouser.com/Optoelectronics/LED-Lighting/LED-Emitters/Standard-LEDs-SMD/_/N-b1bb1?P=1yzvlkxZ1yopobeZ1yuo9ge) everywhere.

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

---

