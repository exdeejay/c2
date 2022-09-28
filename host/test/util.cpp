#include "gtest/gtest.h"
#include "util.h"

TEST(UtilTest, Byteswap) {
    uint32_t val = 0x000000ff;
    EXPECT_EQ(byteswap(val), 0xff000000);
    val = 0x0000ff00;
    EXPECT_EQ(byteswap(val), 0x00ff0000);
    val = 0x00ff0000;
    EXPECT_EQ(byteswap(val), 0x0000ff00);
    val = 0xff000000;
    EXPECT_EQ(byteswap(val), 0x000000ff);
    val = 0x12345678;
    EXPECT_EQ(byteswap(val), 0x78563412);

    uint64_t val64 = 0x1234567812345678ull;
    EXPECT_EQ(byteswap(val64), 0x7856341278563412ull);
}

TEST(UtilTest, CombineDWORDs) {
    EXPECT_EQ(combineDWORDs(0x12345678, 0x12345678), 0x1234567812345678ull);
    EXPECT_EQ(combineDWORDs(0xffffffff, 0x00000000), 0xffffffff00000000ull);
}
