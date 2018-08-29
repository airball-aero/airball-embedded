#include "xbee_api_payload.h"

#include <gtest/gtest.h>
#include <string>

// The following test data was generated using the Frames Generator in the Digi
// XCTU tool and copied in here.

airball::xbee_api_frame read(const uint8_t* data, const size_t size) {
  size_t i = 0;
  return airball::xbee_api_frame::from_bytes(
      [&](char* p, size_t len) {
        memcpy(p, data + i, len);
        i += len;
      });
}

std::string write_and_compare(
    const airball::xbee_api_frame& frame,
    const uint8_t* data,
    const size_t size) {
  std::string got = frame.to_bytes();
  std::string want((const char*) data, size);
  EXPECT_EQ(got, want);
}

TEST(xbee_api_payload, x90_receive_64_bit) {
  uint8_t data[] {
      0x7E, 0x00, 0x11, 0x90, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0x01, 0x66, 0x6C, 0x6F, 0x6F, 0x70, 0x58,
  };
  airball::x90_receive_64_bit p(read(data, sizeof(data)));
  EXPECT_EQ(p.frame().api(), 0x90);
  EXPECT_EQ(p.source_address_64_bit(), 0xffffffffffffffff);
  EXPECT_EQ(p.source_address_16_bit(), 0xffff);
  EXPECT_EQ(p.options(), 0x01);
  EXPECT_EQ(p.data(), "floop");
}

TEST(xbee_api_payload, x81_receive_16_bit) {
  uint8_t data[]{
      0x7E, 0x00, 0x0A, 0x81, 0xFA, 0xFE, 0xED, 0x3B, 0x66, 0x6C, 0x6F, 0x6F,
      0x70, 0x3E,
  };
  airball::x81_receive_16_bit p(read(data, sizeof(data)));
  EXPECT_EQ(p.frame().api(), 0x81);
  EXPECT_EQ(p.source_address(), 0xfafe);
  EXPECT_EQ(p.rssi(), 0xed);
  EXPECT_EQ(p.options(), 0x3b);
  EXPECT_EQ(p.data(), "floop");
}

TEST(xbee_api_payload, x01_send_16_bit) {
  uint8_t data[]{
      0x7E, 0x00, 0x0A, 0x01, 0x0A, 0xFA, 0xFE, 0x08, 0x66, 0x6C, 0x6F, 0x6F,
      0x70, 0xD4,
  };
  airball::x01_send_16_bit p(
      0x0a,
      0xfafe,
      0x08,
      "floop");
  write_and_compare(p.frame(), data, sizeof(data));
}

TEST(xbee_api_payload, x10_send_64_bit) {
  uint8_t data[] {
      0x7E, 0x00, 0x13, 0x10, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
      0xFF, 0xFF, 0xFF, 0x0B, 0x0C, 0x66, 0x6C, 0x6F, 0x6F, 0x70, 0xB2,
  };
  airball::x10_send_64_bit p(
      0x0a,
      0x000000000000ffff,
      0xffff,
      0x0b,
      0x0c,
      "floop");
  write_and_compare(p.frame(), data, sizeof(data));
}
