#ifndef WIFI_INTERFACE_H
#define WIFI_INTERFACE_H

#include <string>

class WifiInterface {
 public:
  virtual void begin() = 0;
  virtual void send(const char* sentence) = 0;
};

#endif // WIFI_INTERFACE_H
