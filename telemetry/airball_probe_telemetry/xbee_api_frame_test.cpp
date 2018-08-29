#include "xbee_api_frame.h"

#include <gtest/gtest.h>
#include <string>

// The following test data was generated using the Frames Generator in the Digi
// XCTU tool and copied in here.

const uint8_t test_api = 0x10;

const uint8_t test_data[] {
    0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff,
    0x00,
    0x00,
    'f', 'l', 'o', 'o', 'p',
};

const uint8_t test_result_bytes[] {
    0x7E,
    0x00,
    0x13, 0x10,
    0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF,
    0x00,
    0x00,
    0x66, 0x6C, 0x6F, 0x6F, 0x70,
    0xD0,
};

const uint8_t test_checksum = 0xd0;

std::string test_data_string() {
  return std::string((char*) test_data, sizeof(test_data));
}

std::string test_result_bytes_string() {
  return std::string((char*) test_result_bytes, sizeof(test_result_bytes));
}

TEST(xbee_api_frame, direct_constructor) {
  airball::xbee_api_frame f(test_api, test_data_string());
  EXPECT_EQ(f.api(), test_api);
  EXPECT_EQ(f.payload(), test_data_string());
  EXPECT_EQ(f.checksum(), test_checksum);
}

TEST(xbee_api_frame, to_bytes) {
  airball::xbee_api_frame f(test_api, test_data_string());
  EXPECT_EQ(test_result_bytes_string(), f.to_bytes());
}

TEST(xbee_api_frame, from_bytes) {
  size_t i = 0;
  airball::xbee_api_frame f = airball::xbee_api_frame::from_bytes(
      [&](char* p, size_t len) {
        memcpy(p, test_result_bytes + i, len);
        i += len;
      });
  EXPECT_EQ(f.api(), test_api);
  EXPECT_EQ(f.payload(), test_data_string());
  EXPECT_EQ(f.checksum(), test_checksum);
}
