# Changes in v5 #

## TODO ##

* Invert the connections of the USB Tx/Rx LEDs: connect them between the relevant pins and `VCC_3V3` instead of `GND` so that they are *off* most of the time instead of *on*.
* Remove the LDO from series connection with the battery charger; use a MOSFET to switch `BAT+` versus `USB_5V0` power to both directly.
* Connect the XBee's `SLEEP_RQ` pin to a GPIO on the ATmega32u4 so that it can be put in power save modes programmatically.
* Re-arrange the battery status LED connections. They were incorrectly wired to `BAT+` instead of `USB_5V0` which caused the "Chg." LED to be lit any time the power switch was on. Additionally the "Full" LED was wired incorrectly and could not work at all.
* Add supervisor IC (MAX809?) and MOSFET (DMN3300U?) to allow safe cutoff before battery is fully drained?
* Add a "power" LED (since with radio sleep and USB LED fixes, all LEDs may be off in some cases).

# Changes in v4 (2018-02-28) #

* Integrated the necessary Sparkfun Fio v3 components onto the sensor board itself to obviate the need for the Fio.
* Added a DS2782 battery monitor IC.
* Upgraded to a beefier LT1963 3.3V LDO.
* Relocated the pressure sensors to a two side-by-side configuration with ports facing the front of the probe.
* Replaced the TMP102 SOT563 temperature sensor with a TMP275 SOIC8 sensor so that it's much easier to assemble.
* Added many status LEDs.

# Changes in v3 (2018-01-18) #

* Reversed the Sparkfun Fio v3 socket to match reality in its upside down mounting which was wrong in v2 (oops).

# Changes in v2 (2018-01-12) #

* Major cleanups to all footprints, labels, and part orientation. (Kept the original positioning of pressure sensors, overall shape, and part selection.)
* Re-drew the schematic.

# Initial version v1 #

* Initial version created by Ihab.
