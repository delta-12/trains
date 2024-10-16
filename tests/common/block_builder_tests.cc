/*****************************************************************************
 * @file block_builder_tests.cc
 *
 * @brief Unit testing for block builder.
 *****************************************************************************/

#include <gtest/gtest.h>

#include "csv_parser.h"
#include "block_builder.h"

TEST(BlockBuilderTests, BlueBline)
{
    std::string input_path = "C:\\Users\\truon\\Downloads\\blue_line.csv";
    std::filesystem::path path(input_path);
    CsvParser parser(path);
    BlockBuilder bb(parser.GetRecords());

    ASSERT_EQ(bb.GetSize(), 15);
    ASSERT_EQ(bb.GetBlock(1).block, 1);
    ASSERT_EQ(bb.GetBlock(2).block, 2);
    ASSERT_EQ(bb.GetBlock(2).has_crossing, false);

    ASSERT_EQ(bb.GetBlock(3).has_crossing, true);
    ASSERT_EQ(bb.GetBlock(5).has_switch, true);
    ASSERT_EQ(bb.GetBlock(6).has_switch, true);
    ASSERT_EQ(bb.GetBlock(10).has_station, true);
    ASSERT_EQ(bb.GetBlock(15).has_station, true);
}

TEST(BlockBuilderTests, GreenLine)
{
    std::string input_path = "C:\\Users\\truon\\Downloads\\GreenLine.csv";
    std::filesystem::path path(input_path);
    CsvParser parser(path);
    BlockBuilder bb(parser.GetRecords());

    ASSERT_EQ(bb.GetSize(), 150);
    ASSERT_EQ(bb.GetBlock(2).block, 2);
    ASSERT_EQ(bb.GetBlock(1).has_crossing, false);

    ASSERT_EQ(bb.GetBlock(2).has_crossing, false);
    ASSERT_EQ(bb.GetBlock(2).has_switch, false);
    ASSERT_EQ(bb.GetBlock(2).has_station, true);
    ASSERT_EQ(bb.GetBlock(2).has_light, false);

    ASSERT_EQ(bb.GetBlock(62).has_crossing, false);
    ASSERT_EQ(bb.GetBlock(62).has_switch, true);
    ASSERT_EQ(bb.GetBlock(62).has_station, false);
    ASSERT_EQ(bb.GetBlock(62).has_light, false);

    ASSERT_EQ(bb.GetBlock(77).station_side, types::StationSide::BOTH);
}