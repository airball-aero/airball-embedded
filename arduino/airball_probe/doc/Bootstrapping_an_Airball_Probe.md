# Bootstrapping an Airball Probe

Once a probe has been built (or if it has been damaged or "bricked" with bad firmware) it needs to be bootstrapped with the Arduino bootloader. The [Atmel ATmega32u4](https://www.microchip.com/wwwproducts/en/ATmega32U4) microcontroller it has onboard does not contain any firmware (or contains a broken one), and thus there is no way to get firmware *onto* it using the Arduino IDE.

 An AVR in-circuit serial programmer (ICSP or ISP) device must be connected to the 6-pin "ICSP" connector on the probe to do so. There are many compatible programmers available, including the [Microchip Atmel-ICE](https://www.microchip.com/developmenttools/ProductDetails/atatmel-ice) ([$133.90 from DigiKey](https://www.digikey.com/product-detail/en/microchip-technology/ATATMEL-ICE/ATATMEL-ICE-ND/4753379)), [Sparkfun's Pocket AVR](https://www.sparkfun.com/products/9825) ($14.95), and many more. These are useful to have in any case when working with Arduino, as they allow the Atmel ATmega devices to be re-programmed as necessary.

## Install `avrdude`

The `avrdude` program will be used to program the microcontroller. Installing it is outside the scope of this document, but it is available from various free software respositories such as Ubuntu's and the `brew` repository available for Mac OS X.

## Get your AVR ICSP programmer working with `avrdude`

### Identify your programmer and its port

The `avrdude` program needs to know what kind of programmer you're using. You can get a list of supported programmers by running `avrdude -c .`. For example:

* `atmelice_isp` – The Microchip Atmel-ICE, in ISP mode.
* `usbtiny` – Supports many programmers including the Sparkfun Pocket AVR.

This name is then passed to `avrdude` with the `-c` argument. The examples below are using the Atmel AVR Dragon, in ISP mode, so they use `avrdude -c dragon_isp`.

### Connect the programmer to your Airball Probe

Identify the ICSP port, which is a 3x2-pin IDC connector labeled (helpfully) **ICSP**. It has a subtle white bar under pin 1 (just to the left of the "I" in "ICSP"), which is the *MISO* signal. Connect a 6-pin AVR ICSP/ISP cable to this, and connect the other end to the programmer.

The ICSP programming cable does not provide power, so it requires that the board be powered by some other means &mdash; either USB or its on-board battery &mdash; and for the power switch to be ON.

### Verify your programmer can speak to the Airball Probe

```
avrdude -c <PROGRAMMER> -p m32u4
```

The output of this command should look like (with `dragon_isp`):

```
$ avrdude -c dragon_isp -p m32u4 

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.15s

avrdude: Device signature = 0x1e9587 (probably m32u4)

avrdude: safemode: Fuses OK (E:CE, H:D8, L:FF)

avrdude done.  Thank you.
```

This implies that the programmer was able to correctly communicate with the microcontroller on the Airball Probe board, and correctly identified it as an Atmel ATmega32u4 (`m32u4`). If it is unable to do so (or doesn't identify the `Device signature` correctly), it's not worthwhile to continue following these steps before troubleshooting. It's okay if the `Fuses OK` values above do not match your device, as they will not match if it has not been programmed yet.

## Program the Airball Probe with a bootloader

Since the Airball Probe uses the same Atmel ATmega32u4 microcontroller as the [Sparkfun Fio v3](https://www.sparkfun.com/products/11520), it is compatible and uses its bootloader for programming support in the Arduino IDE.

### Clone the Sparkfun Arduino Boards repository

You'll need to get the *Arduino_Boards* GitHub repository from Sparkfun, which contains the firmware for all Sparkfun-designed Arduino boards:

```
git clone https://github.com/sparkfun/Arduino_Boards
```

Alternatively, you could [download just the file](https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/sparkfun/avr/bootloaders/caterina/Caterina-fio.hex) instead.

### Write the Sparkfun Fio v3 bootloader to flash

Programming the microcontroller actually is two separate actions:

1. Writing the `.hex` file to the on-board Flash memory, containing the bootloader.
2. Setting the high, low, and extended fuses (hfuse, lfuse, efuse) to appropriate values.

The following command will accomplish this for the Airball Probe:

```
avrdude -c dragon_isp -p m32u4 \
  -U flash:w:Arduino_Boards/sparkfun/avr/bootloaders/caterina/Caterina-fio.hex \
  -U hfuse:w:0xd8:m -U lfuse:w:0xff:m -U efuse:w:0xce:m
```

## Verify that the bootloader works

Follow the instructions to [Program an Airball Probe](Program_an_Airball_Probe.md) to write the Airball firmware to it.

# Done!
