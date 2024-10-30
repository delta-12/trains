/*****************************************************************************
* @file block_builder_tests.cc
*
* @brief Unit testing for block builder.
*****************************************************************************/

#include <gtest/gtest.h>

#include "csv_parser.h"
#include "block_builder.h"
#include "types.h"

TEST(BlockBuilderTests, GetSize)
{
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser             parser(path);
    BlockBuilder          bb(parser.GetRecords(), Module::MODULE_TRACK_MODEL);
    ASSERT_EQ(bb.GetSize(), 151);

    std::vector<types::Block> result = bb.GetBlocks();
    ASSERT_EQ(result.size(), 151);
    bb.Reset();
    ASSERT_EQ(bb.GetSize(), 0);
}

TEST(BlockBuilderTests, ConvertRecordToBlock)
{
    std::filesystem::path                 base_path = std::filesystem::current_path();
    std::filesystem::path                 path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser                             parser(path);
    std::vector<std::vector<std::string>> records = parser.GetRecords();
    BlockBuilder                          bb;
    types::Block                          block = bb.ConvertRecordToBlock(records[89]);

    ASSERT_EQ(block.has_station, true);
    ASSERT_EQ(block.station_name, "Poplar");
}

TEST(BlockBuilderTests, ConvertRecordToBlockCTC)
{
    std::filesystem::path                 base_path = std::filesystem::current_path();
    std::filesystem::path                 path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_v4.csv";
    CsvParser                             parser(path);
    std::vector<std::vector<std::string>> records = parser.GetRecords();
    BlockBuilder                          bb;
    types::Block                          block = bb.ConvertRecordToBlockCTC(records[73]);

    ASSERT_EQ(block.has_station, true);
    ASSERT_EQ(block.station_name, "Dormont");
    ASSERT_EQ(static_cast<int>(block.total_time_to_station.count()), 150);
}

TEST(BlockBuilderTests, AssignBlockInfrastructure)
{
    std::filesystem::path                 base_path = std::filesystem::current_path();
    std::filesystem::path                 path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser                             parser(path);
    std::vector<std::vector<std::string>> records = parser.GetRecords();

    BlockBuilder bb;
    types::Block block = bb.ConvertRecordToBlock(records[89]);

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
    BlockBuilder          bb(parser.GetRecords(), Module::MODULE_TRACK_MODEL);
    types::Block          block;

    ASSERT_EQ(bb.GetSize(), 16);
    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(1, block));
    ASSERT_EQ(block.block, 1);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(2, block));
    ASSERT_EQ(block.block, 2);
    ASSERT_EQ(block.has_crossing, false);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(3, block));
    ASSERT_EQ(block.has_crossing, true);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(5, block));
    ASSERT_EQ(block.has_switch, true);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(6, block));
    ASSERT_EQ(block.has_switch, true);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(10, block));
    ASSERT_EQ(block.station_name, "Station b");
    ASSERT_EQ(block.has_station, true);
    ASSERT_EQ(block.has_light, false);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(15, block));
    ASSERT_EQ(block.has_station, true);

    ASSERT_EQ(types::ERROR_INVALID_BLOCK, bb.GetBlock(16, block));
}

TEST(BlockBuilderTests, GreenLine)
{
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser             parser(path);
    BlockBuilder          bb(parser.GetRecords(), Module::MODULE_TRACK_MODEL);
    types::Block          block;

    ASSERT_EQ(bb.GetSize(), 151);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(1, block));
    ASSERT_EQ(block.has_crossing, false);
    ASSERT_EQ(block.has_switch, true);
    ASSERT_EQ(block.direction, types::BLOCKDIRECTION_UNIDIRECTIONAL);
    std::cout << block.direction;
    ASSERT_EQ(block.switch_connection, 13);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(2, block));
    ASSERT_EQ(block.block, 2);
    ASSERT_EQ(block.has_crossing, false);
    ASSERT_EQ(block.has_switch, false);
    ASSERT_EQ(block.has_station, true);
    ASSERT_EQ(block.has_light, false);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(62, block));
    ASSERT_EQ(block.has_crossing, false);
    ASSERT_EQ(block.has_switch, false);
    ASSERT_EQ(block.direction, types::BLOCKDIRECTION_UNIDIRECTIONAL);
    ASSERT_EQ(block.has_station, false);
    ASSERT_EQ(block.has_light, false);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(77, block));
    ASSERT_EQ(block.station_side, types::StationSide::STATIONSIDE_BOTH);

    ASSERT_EQ(types::ERROR_INVALID_BLOCK, bb.GetBlock(-1, block));
    ASSERT_EQ(types::ERROR_INVALID_BLOCK, bb.GetBlock(151, block));
}

TEST(BlockBuilderTests, GreenLineV4)
{
    std::filesystem::path                 base_path = std::filesystem::current_path();
    std::filesystem::path                 path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_v4.csv";
    CsvParser                             parser(path);
    std::vector<std::vector<std::string>> records = parser.GetRecords();
    BlockBuilder                          bb(parser.GetRecords(), Module::MODULE_CTC);

    types::Block block;
    ASSERT_EQ(bb.GetSize(), 150);
    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(16, block));
    ASSERT_EQ(block.has_station, true);
    ASSERT_EQ(block.station_name, "Downtown");

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(141, block));
    ASSERT_EQ(block.has_station, true);
    ASSERT_EQ(block.station_name, "Central");
    ASSERT_EQ(block.underground, true);
}