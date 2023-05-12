#include "wifi_client.h"

#define WIFI_SSID "airball0011"
#define WIFI_PASS "relativewind"
#define WIFI_UDP_PORT 30123

// Broadcast IP is hard coded to the expected network we
// will join. TODO: Figure this out dynamically.
IPAddress broadcast_ip(192, 168, 4, 255);

void wifi_station_connected(WiFiEvent_t event, WiFiEventInfo_t info) { 
  Serial.println("wifi_station_connected");
}

void wifi_got_ip(WiFiEvent_t event, WiFiEventInfo_t info) { 
  Serial.println("wifi_got_ip");
  Serial.printf("WiFi.localIp() = %s\n", WiFi.localIP().toString());
}

void wifi_station_disconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("wifi_station_disconnected");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

WifiClient::WifiClient() {}

// See advice on WiFi reconnection here:
// https://stackoverflow.com/questions/73485702/esp32-event-based-reconnect-to-wifi-on-connection-lost-disconnect

void WifiClient::begin() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.onEvent(wifi_station_connected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(wifi_got_ip, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(wifi_station_disconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED); 
}

void WifiClient::send(const char* sentence) {
  if (!WiFi.isConnected()) return;
  Serial.printf("wifi_send %s\n", sentence);
  wifi_udp.beginPacket(broadcast_ip, WIFI_UDP_PORT);
  wifi_udp.write((const uint8_t*) sentence, strlen(sentence));
  wifi_udp.endPacket();
}
