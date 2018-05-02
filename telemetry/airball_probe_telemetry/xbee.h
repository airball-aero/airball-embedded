#ifndef XBEE_H
#define XBEE_H

#include <string>
#include <asio/serial_port.hpp>
#include <asio/streambuf.hpp>
#include <istream>
#include <asio/read_until.hpp>
#include <asio/write.hpp>
#include <asio/read.hpp>
#include <iostream>
#include <asio/impl/read.hpp>
#include <asio/impl/write.hpp>
#include "xbee_packet.h"

class xbee {
private:
    std::string device_filename;
    unsigned int baud_rate;

    asio::io_service io_service;
    asio::serial_port serial_port;
    asio::streambuf input_data_buffer, output_data_buffer;
    std::istream input_buffer;
    std::ostream output_buffer;
  uint16_t write_checksum_sum = 0;

    static const char START_DELIMITER = 0x7e;

public:

    xbee(std::string serial_device_filename, unsigned int baud_rate) :
            device_filename(serial_device_filename),
            baud_rate(baud_rate),
            serial_port(io_service),
            input_buffer(&input_data_buffer),
            output_buffer(&output_data_buffer) {

        serial_port.open(serial_device_filename);
        serial_port.set_option(asio::serial_port_base::baud_rate(baud_rate));
        serial_port.set_option(asio::serial_port_base::character_size(8));
        serial_port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
        serial_port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
        serial_port.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
    }

    void put(std::string str) {
        std::cout << "Sending: " << str << std::endl;
        output_buffer << str;
        asio::write(serial_port, output_data_buffer);
    }

    void discard_until(char c) {
        char cr;
        do {
            serial_port.read_some(asio::buffer(&cr, 1));
        } while (cr != c);
    }

    void discard_until(char *str) {
        int p = 0;
        int l = strlen(str)-1;
        std::cout << "Trying to discard data" << std::endl;
        asio::error_code ec;
        do {
            char c;
            std::cout << "Trying to read" << std::endl;
            size_t len = serial_port.read_some(asio::buffer(&c, 1), ec);

            std::cout << "Got data to discard: " << len << ": " << c << std::endl;
            std::cout << "Error code: " << ec.value() << ", message: " << ec.message() << std::endl;

            if (str[p] == c) {
                std::cout << "Found " << c << " moving to next char " << str[p+1] << std::endl;
                if (p == l) {
                    std::cout << "Got a match" << std::endl;
                    return;
                }
                p++;
            } else {
                std::cout << "No match" << std::endl;
                p = 0;
            }

        } while (ec.value() != asio::error::misc_errors::eof);
    }

    std::string get_line(const char end = '\n') {
        char line[512] = {0};
        asio::read_until(serial_port, input_data_buffer, end);
        input_buffer.getline(line, sizeof(line));
        std::cout << "Got line: " << line << std::endl;
        return std::string(line);
    }

    std::string read_until(const char end = 0x7e) {
        char line[512] = {0};
        asio::read_until(serial_port, input_data_buffer, end);
        input_buffer.getline(line, sizeof(line));
        std::cout << "Got line: " << line << std::endl;
        return std::string(line);
    }

    xbee_packet read_packet() {
        // Synchronize with the beginning of an API packet.
        discard_until(START_DELIMITER);

        xbee_packet packet;

        uint8_t x[2];
        asio::read(serial_port, asio::buffer(x, 2), asio::transfer_all());

        packet.length = ((uint16_t)x[0] << 8) | (uint16_t)x[1];

        packet.frame_data.resize(packet.length-1);
        asio::read(serial_port, asio::buffer(&packet.frame_type, 1), asio::transfer_all());
        asio::read(serial_port, asio::buffer(packet.frame_data.data(), (size_t) packet.length-1), asio::transfer_all());
        asio::read(serial_port, asio::buffer(&packet.checksum, 1), asio::transfer_all());

        return packet;
    }

    void enterCommandMode(unsigned int guard_time=1200) {
        usleep(guard_time * 1000);
        put("+++");
        usleep(guard_time * 1000);
    }

    void sendCommand(std::string command) {
        put(command + "\r");
    }

    void exitCommandMode() {
        sendCommand("ATCN");
    }

  void write(char c) {
    write_checksum_sum += c;
    asio::write(serial_port, asio::buffer(&c, 1));
  }

  void write_checksum() {
    write(0xff - (uint8_t)(write_checksum_sum & 0xff));
  }

  void write(const char *s, int len) {
    for(size_t i=0; i < len; i++) {
      write(s[i]);
    }
  }

  void write_uint8(uint8_t value) {
    write(value);
  }

  void write_uint16(uint16_t value) {
    write((uint8_t) (value & 0xff00 >> 8));   // MSB
    write((uint8_t) (value & 0x00ff));        // LSB
  }

  void reset_checksum() {
    write_checksum_sum = 0;
  }

  void send_packet(uint16_t destination, const std::string& buf) {
    write_uint8(0x7e); // Start
    write_uint16(buf.length() + 1 + 1 + 2 + 1);

    reset_checksum();
    write_uint8(0x01); // API ID
    write_uint8(0x00); // Frame ID
    write_uint16(destination); // Destination Address
    write_uint8(0x00); // Options
    write(buf.c_str(), buf.length()); // Data
    write_checksum(); // Checksum
  }
};

#endif // XBEE_H
