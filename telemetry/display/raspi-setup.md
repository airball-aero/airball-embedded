# Raspberry Pi setup for Airball display

The Airball display executable, `ab_device`, is called with five
arguments, which are supplied in sequence. The "usual" values are the
ones that work with our default Raspberry Pi hardware configuration.

| Arg | Meaning            | Usual value                               |
|-----|--------------------|-------------------------------------------|
| 1   | XBee type          | `0` for 802.14 XBee, `1` for 900 MHz Xbee |
| 2   | XBee serial device | `/dev/ttyAMA0`                            |
| 3   | Encoder push GPIO  | `2`                                       |
| 4   | Encoder A GPIO     | `3`                                       |
| 5   | Encoder B GPIO     | `4`                                       |

The most commmon command line invocation of the binary (from startup
scripts, etc.) is therefore:

```
ab_device 0 /dev/ttyAMAO 2 3 4
```