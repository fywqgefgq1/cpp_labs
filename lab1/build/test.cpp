#include "bitarray.h"
#include "gtest/gtest.h"

namespace {
    TEST(BitArrayTest, empty_init_checkup)
    {
        BitArray TestArray;
        EXPECT_EQ(TestArray.size(), 0);
        EXPECT_EQ(TestArray.empty(), true);
    }

    TEST(BitArrayTest, init_checkup)
    {
        BitArray TestArray(16, 0b0110000101100010);
        EXPECT_EQ(TestArray.size(), 2);
        EXPECT_EQ(TestArray.empty(), false);
        EXPECT_EQ(TestArray.any(), true);
        EXPECT_EQ(TestArray.count(), 6);
    }

    TEST(BitArrayTest, equal_test)
    {
        BitArray TestArray(16, 0b0110000101100010);
        EXPECT_EQ((TestArray == BitArray(16, 0b0110000101100010)), true);
        EXPECT_EQ((TestArray != BitArray(16, 0b0110000101100010)), false);
    }

    TEST(BitArrayTest, assignment_test)
    {
        BitArray TestArray(16, 0b0110000101100010);
        BitArray TestArray2(16, 0b0110000101100011);
        TestArray = TestArray2;
        EXPECT_EQ((TestArray == TestArray2), true);
    }

    TEST(BitArrayTest, set_reset)
    {
        BitArray TestArray(16, 0b0110000101100010);
        TestArray.set(0, 1);
        TestArray.reset(13);
        BitArray TestArray2(16, 0b0100000101100011);
        EXPECT_EQ((TestArray == TestArray2), true);
        TestArray.set();
        EXPECT_EQ((TestArray == BitArray(16, 0xFFFF)), true);
        TestArray.reset();
        EXPECT_EQ((TestArray == BitArray(16, 0x0000)), true);
    }

    TEST(BitArrayTest, swap)
    {
        BitArray TestArray(16, 0b0110000101100010);
        BitArray TestArray2(16, 0b0100000101100011);
        TestArray.swap(TestArray2);
        EXPECT_EQ(((TestArray == BitArray(16, 0b0100000101100011)) && 
        (TestArray2 == BitArray(16, 0b0110000101100010))), true);
    }

    TEST(BitArrayTest, resize_pushback)
    {
        BitArray TestArray(16, 0b0110000101100010);
        TestArray.resize(20, 0);
        EXPECT_EQ((TestArray == BitArray(20, 0b00000110000101100010)), true);
        TestArray.resize(12);
        EXPECT_EQ((TestArray == BitArray(12, 0b000101100010)), true);
        TestArray.push_back(1);
        TestArray.push_back(0);
        EXPECT_EQ((TestArray == BitArray(14, 0b01000101100010)), true);
    }

    TEST(BitArrayTest, to_string)
    {
        BitArray TestArray(16, 0b0110000101100010);
        EXPECT_EQ(TestArray.to_string(true, ' '), "01100001 01100010");

        TestArray = BitArray(22, 0b0001110110000101100010);
        EXPECT_EQ(TestArray.to_string(true, '_'), "000111_01100001_01100010");
    }

    TEST(BitArrayTest, operators_test)
    {
        BitArray TestArray1(16, 0b0110000101100010);
        BitArray TestArray2(16, 0b0110111001100111);
        EXPECT_EQ((TestArray1 | TestArray2).to_string(true, '_'), "01111111_01100111");
        EXPECT_EQ((TestArray1 & TestArray2).to_string(true, '_'), "01100000_01100010");
        EXPECT_EQ((TestArray1 ^ TestArray2).to_string(true, '_'), "00001111_00000101");
        EXPECT_EQ((~TestArray1).to_string(true, '_'), "10011110_10011101");
        EXPECT_EQ((~TestArray2).to_string(true, '_'), "10010001_10011000");
    }

    TEST(BitArrayTest, shifts_test)
    {
        BitArray TestArray(16, 0b0110111001100111);
        EXPECT_EQ((TestArray>>4).to_string(true, '_'), "00000110_11100110");
        EXPECT_EQ((TestArray>>8).to_string(true, '_'), "00000000_01101110");
        EXPECT_EQ((TestArray>>16).to_string(true, '_'), "00000000_00000000");
        EXPECT_EQ((TestArray<<4).to_string(true, '_'), "11100110_01110000");
        EXPECT_EQ((TestArray<<8).to_string(true, '_'), "01100111_00000000");
        EXPECT_EQ((TestArray<<16).to_string(true, '_'), "00000000_00000000");
    }
}
