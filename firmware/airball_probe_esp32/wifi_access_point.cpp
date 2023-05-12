#include "WiFiGeneric.h"
#include "wifi_access_point.h"

#define WIFI_SSID "airball0001"
#define WIFI_PASS "relativewind"
#define WIFI_UDP_PORT 30123

#define STARTUP_DELAY_MS 1000

IPAddress local_ip(192, 168, 4, 10);
IPAddress gateway_ip(192, 168, 4, 254);
IPAddress subnet_ip_mask(255, 255, 255, 0);

WifiAccessPoint::WifiAccessPoint() {}

void WifiAccessPoint::begin() {
  WiFi.mode(WIFI_AP);
  delay(STARTUP_DELAY_MS);
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  WiFi.softAPConfig(local_ip, gateway_ip, subnet_ip_mask);
  broadcast_ip = WiFi.broadcastIP();
}

void WifiAccessPoint::send(const char* sentence) {
  wifi_udp.beginPacket(broadcast_ip, WIFI_UDP_PORT);
  wifi_udp.write((const uint8_t*) sentence, strlen(sentence));
  wifi_udp.endPacket();
}
