#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>

#include "wifi_interface.h"

class WifiClient : public WifiInterface {
 public:
  WifiClient();
  void begin() override;
  void send(const char* sentence) override;
private:
  WiFiUDP wifi_udp;
};

#endif  // WIFI_CLIENT_H
