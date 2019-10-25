# Raspberry Pi setup for Airball display

The Airball display executable, `ab_device`, is called with 4
arguments, which are supplied in sequence. The "usual" values are the
ones that work with our default Raspberry Pi hardware configuration.

| Arg | Meaning            | RPi SoM display  | RPi 3 display          |
|-----|--------------------|------------------|------------------------|
| 1   | XBee serial device | `/dev/ttyAMA0`   | `/dev/ttyAMA0`         |
| 2   | Encoder push GPIO  | `9`              | `22`                   |
| 3   | Encoder A GPIO     | `10`             | `21`                   |
| 4   | Encoder B GPIO     | `11`             | `20`                   |

The most commmon command line invocation of the binary (from startup
scripts, etc.) is therefore:

```
ab_device 0 /dev/ttyAMAO 9 10 11
```