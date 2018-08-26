#include "xbee_api_frame.h"

#include <gtest/gtest.h>
#include <string>

TEST(xbee_api_frame, basic) {
  uint8_t data[9] = {
    0x01, // frame ID
    0xff, // dest addr
    0xff, // dest addr
    0x00, // options
    'f', 'l', 'o', 'o', 'p', // chars
  };
  std::string s((char*) data, 9);
  airball::xbee_api_frame f(0x01,  s);
  EXPECT_EQ(f.api(), 0x01);
  EXPECT_EQ(f.payload().length(), 9);
  EXPECT_EQ(
      std::string(f.payload().data() + 4, 5),
      "floop");
  EXPECT_EQ(f.checksum(), 0xdf);
}
