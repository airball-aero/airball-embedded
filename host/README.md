# Airball Telemetry and Display Source

## Introduction

This is the source tree for the Airball telemetry and display
system. It is intended to be compiled on a Linux system. The actual
display runs on a Raspberry Pi, and at the moment the system needs to
be compiled from scratch on the Raspberry Pi as well.

## Configuring monitor

For the display we are using:

https://www.waveshare.com/3.5inch-hdmi-lcd.htm

Add the following to `config.txt` on the boot partition.

```
hdmi_group=2
hdmi_mode=87
hdmi_cvt 480 320 60 6 0 0 0
hdmi_drive=1

avoid_warnings=1
```

The `avoid_warnings` setting is based on the following advice:

https://terminalwiki.com/disable-low-voltage-warning-in-raspberry-pi/

## Building

```
sudo apt-get install \
     git \
     cmake \
     libcairo2-dev \
     libasio-dev \
     libboost-all-dev \
     libasound2-dev \
     libeigen3-dev \

git clone https://github.com/airball-aero/airball-embedded.git

cd airball-embedded/host
mkdir external
git clone https://github.com/google/googletest external/googletest
git clone https://github.com/miloyip/rapidjson external/rapidjson

mkdir build
cd build
cmake ..
make ab
```

## Network setup

We join the Wi-Fi network set up by the probe. We do not try to pull a
DHCP lease; rather, we assign ourselves a static IP address that we
know the probe will not assign to anyone else.

We allow the probe to be the base station because we have had trouble
getting the Raspberry Pi to act as a base station. We assign ourselves
a static address to avoid any delay in getting started. The well-known
address also makes sure that the Raspberry Pi has a predictable IP
address, so that a user can go to that address to get the Web
configuration interface.

### `/etc/wpa_supplicant.conf`

Depending on the "serial number" of the device being configured, the
SSID will be {`airball0001`, `airball0002`, ...}.

```
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=US
network={
  ssid="airball0001"
  psk="relativewind"
}
```

### `/etc/dhcpcd.conf`

Add the following lines:

```
interface wlan0
static ip_address=192.168.4.1/24
```

## System configuration

```
sudo apt install python-pip
sudo pip install flask
```

Copy each of the contents of `rootfs/` into the target system.

## Web settings editor

On a separate system, clone and build:

https://github.com/airball-aero/airball-settings-editor

Build this using:

```
flutter build web --release --web-renderer=html
```

Copy all the _contents_ of `build/web` into a new directory called
`/var/www/app/`.

Edit the file `/var/www/app/index.html` and ensure the `<base>` tag reads:

```
<base href="/app/">
```
