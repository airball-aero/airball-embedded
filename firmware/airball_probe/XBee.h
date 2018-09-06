/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018, Jeremy Cole
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef XBEE_H
#define XBEE_H

// #define XBEE_DEBUG

class XBee {
private:

  Stream *serial = nullptr;
  Stream *debug = nullptr;

  uint16_t write_checksum_sum = 0;

public:

  XBee(Stream *serial) : serial(serial) { }

  void setDebugStream(Stream *debug) {
    this->debug = debug;
  }

  void discard() {
    while(serial->available())
      serial->read();
  }

  void reset_checksum() {
    write_checksum_sum = 0;
  }

  void write(char c) {
    write_checksum_sum += c;
    serial->write(c);
  }

  void write_checksum() {
    serial->write(0xff - (uint8_t)(write_checksum_sum & 0xff));
  }

  void write(char *s) {
    for(size_t i=0; i<strlen(s); i++) {
      write(s[i]);
    }
  }

  void write(char *buf, uint8_t buf_len) {
    for(int i=0; i<buf_len; i++) {
      write(buf[i]);
    }
  }

  bool waitForResponse(char *buf, size_t buf_len) {
    size_t len = serial->readBytesUntil('\r', buf, buf_len);

  #ifdef XBEE_DEBUG
  if (debug) {
    debug->print("<XBee (");
    debug->print(len);
    debug->print(" bytes): ");
    debug->write(buf, len);
    debug->println();
  }
  #endif

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
    // Send the "+++" escape sequence surrounded by guard times (>1s by default).
    delay(guard_time);
    serial->write("+++");
    delay(guard_time);

#ifdef XBEE_DEBUG
    if (debug) {
      debug->print("Discarding XBee buffer (");
      debug->print(Serial1.available());
      debug->println(")...");
    }
#endif

    while(serial->available()) {
      serial->read(); // Discard the data.
    }

  }

  bool sendCommand(char const *command) {
#ifdef XBEE_DEBUG
    if (debug) {
      debug->print(">XBee: ");
      debug->print(command);
      debug->println();
    }
#endif

    serial->print(command);
    serial->print('\r');

    return waitForOK();
  }

  void exitCommandMode() {
    sendCommand("ATCN");
  }

  uint8_t checksum(char *buf, uint8_t buf_len) {
    uint16_t sum = 0;
    for (int i=0; i<buf_len; i++) {
      sum += buf[i];
    }
    return 0xff - (sum & 0xff);
  }

  void write_uint8(uint8_t value) {
    write(value);
  }

  void write_uint16(uint16_t value) {
    write((uint8_t) (value & 0xff00 >> 8));   // MSB
    write((uint8_t) (value & 0x00ff));        // LSB
  }

  void send_packet(uint16_t destination, char *buf, uint8_t buf_len) {
    write_uint8(0x7e); // Start
    write_uint16(buf_len + 1 + 1 + 2 + 1);

    reset_checksum();
    write_uint8(0x01); // API ID
    write_uint8(0x00); // Frame ID
    write_uint16(destination); // Destination Address
    write_uint8(0x00); // Options
    write(buf, buf_len); // Data
    write_checksum(); // Checksum
  }
};

#endif // XBEE_H
