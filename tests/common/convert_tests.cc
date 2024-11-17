/*****************************************************************************
* @file convert_tests.cc
*
* @brief Unit testing for conversions.
*****************************************************************************/

#include <gtest/gtest.h>

#include "convert.h"

TEST(ConvertTests, GetLowByteTest)
{
    ASSERT_EQ(0x34, convert::GetLowByte(0x1234));
}

TEST(ConvertTests, GetHighByteTest)
{
    ASSERT_EQ(0x12, convert::GetHighByte(0x1234));
}

TEST(ConvertTests, MakeUint16Test)
{
    ASSERT_EQ(0x1234, convert::MakeUint16(0x12, 0x34));
}

TEST(ConvertTests, MinuteToMillisecond)
{
    std::string               minute      = "2.5";
    std::chrono::milliseconds millisecond = convert::ConvertMinuteToMilliseconds(minute);
    ASSERT_EQ(millisecond.count(), 150000);

    minute      = "2.7";
    millisecond = convert::ConvertMinuteToMilliseconds(minute);
    ASSERT_EQ(millisecond.count(), 162000);

    minute      = "3.2";
    millisecond = convert::ConvertMinuteToMilliseconds(minute);
    ASSERT_EQ(millisecond.count(), 192000);
}