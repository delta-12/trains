/*****************************************************************************
* @file block_builder_tests.cc
*
* @brief Unit testing for block builder.
*****************************************************************************/

#include <gtest/gtest.h>

#include "csv_parser.h"
#include "block_builder.h"

TEST(BlockBuilderTests, GetSize)
{
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser             parser(path);
    BlockBuilder          bb(parser.GetRecords());
    ASSERT_EQ(bb.GetSize(), 150);

    std::vector<types::Block> result = bb.GetBlocks();
    ASSERT_EQ(result.size(), 150);
    bb.Reset();
    ASSERT_EQ(bb.GetSize(), 0);
}

TEST(BlockBuilderTests, ConvertRecordToBlock)
{
    std::filesystem::path                  base_path = std::filesystem::current_path();
    std::filesystem::path                  path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser                              parser(path);
    std::vector<std::vector<std::string> > records = parser.GetRecords();

    BlockBuilder bb;
    types::Block block = bb.ConvertRecordToBlock(records[88]);

    ASSERT_EQ(block.has_station, true);
    ASSERT_EQ(block.station_name, "Poplar");
}

TEST(BlockBuilderTests, AssignBlockInfrastructure)
{
    std::filesystem::path                  base_path = std::filesystem::current_path();
    std::filesystem::path                  path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser                              parser(path);
    std::vector<std::vector<std::string> > records = parser.GetRecords();

    BlockBuilder bb;
    types::Block block = bb.ConvertRecordToBlock(records[88]);

    ASSERT_EQ(block.has_station, true);
    ASSERT_EQ(block.station_name, "Poplar");


    std::string infrastructure = "STATION; DOWNTOWN; RAILWAY CROSSING; UNDERGROUND; SWITCH; LIGHT";
    bb.AssignBlockInfrastructure(block, infrastructure);
    ASSERT_EQ(block.has_station, true);
    ASSERT_EQ(block.has_crossing, true);
    ASSERT_EQ(block.has_light, true);
    ASSERT_EQ(block.underground, true);
    ASSERT_EQ(block.has_switch, true);
    ASSERT_EQ(block.station_name, "Downtown");
}

TEST(BlockBuilderTests, BlueBline)
{
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "blue_line.csv";
    CsvParser             parser(path);
    BlockBuilder          bb(parser.GetRecords());

    ASSERT_EQ(bb.GetSize(), 15);
    ASSERT_EQ(bb.GetBlock(1).block, 1);
    ASSERT_EQ(bb.GetBlock(2).block, 2);
    ASSERT_EQ(bb.GetBlock(2).has_crossing, false);

    ASSERT_EQ(bb.GetBlock(3).has_crossing, true);
    ASSERT_EQ(bb.GetBlock(5).has_switch, true);
    ASSERT_EQ(bb.GetBlock(6).has_switch, true);
    ASSERT_EQ(bb.GetBlock(10).station_name, "Station b");
    ASSERT_EQ(bb.GetBlock(10).has_station, true);
    ASSERT_EQ(bb.GetBlock(15).has_station, true);
}

TEST(BlockBuilderTests, GreenLine)
{
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser             parser(path);
    BlockBuilder          bb(parser.GetRecords());

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

    ASSERT_EQ(bb.GetBlock(77).station_side, types::StationSide::STATIONSIDE_BOTH);
}