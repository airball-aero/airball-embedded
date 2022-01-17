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
     hostapd \
     dnsmasq

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

* `eth0` on the RPi by default is set up as a DHCP client. It can be
  used for connecting via wired Ethernet for development.
* `wlan0` is configured with a static IP of `192.168.4.1`.
* We configure an access point on `wlan0`:
  * Depending on the "serial number" of the device being configured, the
    SSID will be {`airball0001`, `airball0002`, ...}.
  * The default password is `relativewind`.
* The probe joins the same network statically.
  * The probe assigns itself `191.168.4.200`.
  * The RPi must know not to assign that to anyone.

See also:

https://thepi.io/how-to-use-your-raspberry-pi-as-a-wireless-access-point/

Run:

```
sudo systemctl stop hostapd
sudo systemctl stop dnsmasq
```

Add to `/etc/dhcpcd.conf`:

```
interface wlan0
static ip_address=192.168.4.1/24
denyinterfaces eth0
denyinterfaces wlan0
```

Run:

```
sudo mv /etc/dnsmasq.conf /etc/dnsmasq.conf.orig
```

Put into `/etc/dnsmasq.conf`:

```
interface=wlan0
  dhcp-range=192.168.4.10,192.168.4.100,255.255.255.0,24h
```

Put into `/etc/hostapd/hostapd.conf`:

```
interface=wlan0
# bridge=br0
hw_mode=g
channel=7
wmm_enabled=0
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
ssid=airball0001
wpa_passphrase=relativewind
```

In the file `/etc/default/hostapd` ensure:

```
DAEMON_CONF="/etc/hostapd/hostapd.conf"
```

Run the commands:

```
sudo systemctl disable systemd-resolved
sudo systemctl mask systemd-resolved
sudo systemctl unmask hostapd
sudo systemctl start hostapd
sudo systemctl start dnsmasq
```

## System configuration

```
sudo apt install python-pip
sudo pip install flask
```

Copy each of the contents of `rootfs/` (in this repository) into the
target system.

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
