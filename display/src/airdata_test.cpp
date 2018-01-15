#include "airdata.h"

#include "gtest/gtest.h"

namespace airball {

TEST(Airdata, simplething) {
  Airdata ad;
  ad.update_from_sentence(0.0, 273.0, 10.0, 0.0, 0.0);
  EXPECT_NEAR(0.0, ad.alpha(), 0.001);
  EXPECT_NEAR(0.0, ad.beta(), 0.001);
}

} // namespace airball
