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
    ASSERT_EQ(ctc.GetBlockById(1).speed_limit, 15);

    ASSERT_EQ(ctc.GetBlockById(20).block, 20);
    ASSERT_EQ(ctc.GetBlockById(20).speed_limit, 17);

    ASSERT_EQ(ctc.GetBlockById(50).block, 50);
    ASSERT_EQ(ctc.GetBlockById(50).speed_limit, 19);

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
    ASSERT_EQ(train1.authority.front(), 63);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 64);
    ASSERT_EQ(train1.suggested_speed, 19);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 65);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 66);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 67);

    std::vector<types::BlockId> authority;
    while (!train1.authority.empty())
    {
        authority.push_back(train1.authority.front());
        train1.authority.pop();
    }
    ASSERT_EQ(authority[authority.size() - 1], 105);
}

TEST(CtcBackEndTest, SetBlockStates)
{
    ctc::Ctc              ctc;
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();

    std::vector<types::BlockState> block_states;
    types::BlockState              block_state_1(63, true, false);
    types::BlockState              block_state_2(70, true, true);
    block_states.push_back(block_state_1);
    block_states.push_back(block_state_2);

    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

    ASSERT_EQ(ctc.GetBlockById(63).occupied, true);
    ASSERT_EQ(ctc.GetBlockById(70).occupied, true);
    ASSERT_EQ(ctc.GetFailureBlocks()[0], 70);
}

TEST(CtcBackEndTest, TrainReceiveBlockOccupancy)
{
    ctc::Ctc              ctc;
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();

    ctc.ManualDispatch(105);

    ctc::Train train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 63);
    ASSERT_EQ(train1.current_position, 0);

    std::vector<types::BlockState> block_states;
    types::BlockState              block_state_1(63, true, false);
    block_states.push_back(block_state_1);
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

    train1 = ctc.GetTrainById(1);
    ASSERT_EQ(train1.authority.front(), 64);
    ASSERT_EQ(train1.current_position, 63);
    ASSERT_EQ(train1.suggested_speed, 19);
}

TEST(CtcBackEndTest, GetSuggestedSpeedAndAuthorities)
{
    ctc::Ctc              ctc;
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();
    // Dispatch train to block 70
    ctc.ManualDispatch(70);

    // CTC Received Block Occupancy at 63
    std::vector<types::BlockState> block_states = { types::BlockState(63, true, false) };
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

    std::vector<types::TrackCircuitData> track_circuit_data = ctc.GetSuggestedSpeedsAndAuthorities();
    ASSERT_EQ(track_circuit_data.size(), 1);
    types::TrackCircuitData data = track_circuit_data[0];
    ASSERT_EQ(data.authority, 7);
    ASSERT_EQ(data.block, 63);
    ASSERT_EQ(data.speed, 19);

    // CTC Received Block Occupancy at 64
    block_states[0].block = 64;
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

    track_circuit_data = ctc.GetSuggestedSpeedsAndAuthorities();
    data               = track_circuit_data[0];
    ASSERT_EQ(data.authority, 6);
    ASSERT_EQ(data.block, 64);
    ASSERT_EQ(data.speed, 19);
}
