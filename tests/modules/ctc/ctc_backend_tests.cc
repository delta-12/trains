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
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
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
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();

    ASSERT_EQ(ctc.GetNumStation(), 18);

    std::vector<ctc::Station> station = ctc.GetStations();
    ASSERT_EQ(station[0].block_id, 2);
    ASSERT_EQ(station[0].station_name, "Pioneer");
    int total_time_to_Pioneer = static_cast<int>(station[0].total_time_to_station_with_dwell.count());
    ASSERT_EQ(total_time_to_Pioneer, 138);
}

TEST(CtcBackEndTest, SetManualMode)
{
    ctc::Ctc ctc;
    ctc.SetManualMode();
    ASSERT_EQ(ctc.GetOperationMode(), ctc::MANUAL_MODE);
}

TEST(CtcBackEndTest, ManualDispatchToBlock)
{
    ctc::Ctc              ctc;
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();

    ASSERT_EQ(ctc.GetNumStation(), 18);
    ctc.ManualDispatch(105);
    ctc::Train train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 0);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 63);
    ASSERT_EQ(train1.suggested_speed, 19);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 64);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 65);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 66);

    std::vector<types::BlockId> authority;
    while (!train1.authority.empty())
    {
        authority.push_back(train1.authority.front());
        train1.authority.pop();
    }
    ASSERT_EQ(authority[authority.size() - 1], 105);
}
