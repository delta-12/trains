/*****************************************************************************
* @file ctc_backend_tests.cc
*
* @brief Unit testing for Ctc class.
*****************************************************************************/

#include <gtest/gtest.h>

#include "ctc.h"

TEST(CtcBackEndTest, SetTrackLayout)
{
    ctc::Ctc              ctc;
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_v4.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();

    ASSERT_EQ(ctc.GetBlockById(1).block, 1);
    ASSERT_EQ(ctc.GetBlockById(2).block, 2);
    ASSERT_EQ(ctc.GetBlockById(3).block, 3);
    ASSERT_EQ(ctc.GetBlockById(149).block, 149);
    ASSERT_EQ(ctc.GetBlockById(150).block, 150);

    ASSERT_EQ(ctc.GetBlockById(2).station_name, "Pioneer");
    ASSERT_EQ(ctc.GetBlockById(22).station_name, "Whited");
    ASSERT_EQ(ctc.GetBlockById(31).station_name, "South bank");
    ASSERT_EQ(ctc.GetBlockById(73).station_name, "Dormont");
}

TEST(CtcBackEndTest, SetStations)
{
    ctc::Ctc              ctc;
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_v4.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();

    ASSERT_EQ(ctc.GetNumStation(), 18);

    std::vector<ctc::Station> station = ctc.GetStations();
    ASSERT_EQ(station[0].block_id, 2);
    ASSERT_EQ(station[0].station_name, "Pioneer");
    int total_time_to_Pioneer = static_cast<int>(station[0].total_time_to_station_with_dwell.count());
    ASSERT_EQ(total_time_to_Pioneer, 138);
}

TEST(CtcBackEndTest, Authority)
{
    ctc::Ctc              ctc;
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_v4.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();
    // Dispatch Train from yard to block 73 (Dormont)
    ctc.ManualDispatch(73);
    ASSERT_EQ(ctc.GetTrainAuthority(1), 12);

    ctc::Train train1    = ctc.GetTrainById(1);
    std::queue authority = train1.authority;
    ASSERT_EQ(authority.front(), 0);
    authority.pop();
    ASSERT_EQ(authority.front(), 63);
    authority.pop();
    ASSERT_EQ(authority.front(), 64);
    authority.pop();
    ASSERT_EQ(authority.front(), 65);
    authority.pop();
    ASSERT_EQ(authority.front(), 66);
    authority.pop();
    ASSERT_EQ(authority.front(), 67);
    authority.pop();
    ASSERT_EQ(authority.front(), 68);
    authority.pop();
    ASSERT_EQ(authority.front(), 69);
    authority.pop();
    ASSERT_EQ(authority.front(), 70);
    authority.pop();
    ASSERT_EQ(authority.front(), 71);
    authority.pop();
    ASSERT_EQ(authority.front(), 72);
    authority.pop();
    ASSERT_EQ(authority.front(), 73);
    authority.pop();
}

