#ifndef WIFI_ACCESS_POINT_H
#define WIFI_ACCESS_POINT_H

#include <WiFi.h>
#include <WiFiUdp.h>
// #include <WiFiClient.h>

#include "wifi_interface.h"

class WifiAccessPoint : public WifiInterface {
 public:
  WifiAccessPoint();
  void begin() override;
  void send(const char* sentence) override;
private:
  WiFiUDP wifi_udp;
  IPAddress broadcast_ip;
};

#endif  // WIFI_ACCESS_POINT_H
