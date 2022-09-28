#include <gtest/gtest.h>
#include "util.h"

TEST(UtilTest, Byteswap) {
    uint32_t val = 0x000000ff;
    EXPECT_EQ(byteswap(val), 0xff000000);
}
