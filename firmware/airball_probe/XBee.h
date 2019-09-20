#ifndef XBEE_H
#define XBEE_H

class XBee {
private:

  Stream *serial = nullptr;
  uint8_t frame_id_counter = 0;

public:

  XBee(Stream *serial) : serial(serial) { }

  void discard() {
    while(serial->available())
      serial->read();
  }

  void write(char *buf) {
    serial->print(buf);
  }

  bool waitForResponse(char *buf, size_t buf_len) {
    size_t len = serial->readBytesUntil('\r', buf, buf_len);
    return len;
  }

  bool waitForOK() {
    char buf[128];
    size_t len;
    len = waitForResponse(buf, sizeof(buf));
    if (len == 2 && strncmp(buf, "OK", 2) == 0) {
      return true;
    }
    return false;
  }

  void enterCommandMode(int guard_time = 1100) {
    // Send the "+++" escape sequence surrounded by guard times
    // (>1s by default).
    delay(guard_time);
    serial->write("+++");
    delay(guard_time);
    while(serial->available()) {
      serial->read(); // Discard the data.
    }
  }

  bool sendCommand(char const *command) {
    serial->print(command);
    serial->print('\r');
    return waitForOK();
  }

  void exitCommandMode() {
    sendCommand("ATCN");
  }
};

#endif // XBEE_H
