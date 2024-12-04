/*****************************************************************************
* @file ctc_backend_tests.cc
*
* @brief Unit testing for Ctc class.
*****************************************************************************/

#include <gtest/gtest.h>

#include "ctc.h"

static std::string TimePointToString(const std::chrono::system_clock::time_point& time_point);

TEST(CtcBackEndTest, SetTrackLayout)
{
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);

    std::vector<types::Block> blocks = ctc.GetBlocks();
    ASSERT_EQ(blocks.size(), 151);

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
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);

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

TEST(CtcBackEndTest, GetDepartureTime)
{

    TickSource                            tick_source("08:00:00");
    std::shared_ptr<TickSource>           clock = std::make_shared<TickSource>(tick_source);
    ctc::Ctc                              ctc_office(clock);
    std::chrono::system_clock::time_point departure_time;
    std::chrono::duration<double>         travel_time(90);
    ctc_office.SetTrainDepartureTime("10:00:00", travel_time, departure_time);
    ASSERT_EQ(TimePointToString(departure_time), "09:58:30");
}

TEST(CtcBackEndTest, SetSwitchPosition)
{
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);
    ASSERT_EQ(ctc.GetBlockById(12).has_switch, true);
    ASSERT_EQ(ctc.GetBlockById(12).switched, false);
    ctc.SetSwitchPosition(12, true);
    ASSERT_EQ(ctc.GetBlockById(12).switched, true);
    ctc.SetSwitchPosition(12, false);
    ASSERT_EQ(ctc.GetBlockById(12).switched, false);
}

TEST(CtcBackEndTest, DispatchMultipleDestination)
{
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);
    // Dispatch Train regardless of ID
    ctc.ManualDispatch(1, 70);
    ctc::Train   train(0);
    types::Error error = ctc.GetTrainById(1, train);
    ASSERT_EQ(error, types::Error::ERROR_NONE);
    ASSERT_EQ(train.train_id, 1);
    ASSERT_EQ(ctc.GetNumTrains(), 1);
    ASSERT_EQ(train.destination_list.size(), 1);

    // Dispatch Train no matching ID -> new train dispatched making sure train id will always be in increment order
    ctc.ManualDispatch(0, 80);
    ASSERT_EQ(ctc.GetNumTrains(), 2);

    // Add another Destinatino to train 1
    ctc.ManualDispatch(1, 90);
    ASSERT_EQ(ctc.GetNumTrains(), 2);

    error = ctc.GetTrainById(1, train);
    ASSERT_EQ(train.destination_list.size(), 2);
    ASSERT_EQ(train.destination_list[0].destination, 70);
    ASSERT_EQ(train.destination_list[1].destination, 90);
}

TEST(CtcBackEndTest, Authority)
{
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);
    ctc.ManualDispatch(1, 70);
    ASSERT_EQ(ctc.GetTrainAuthority(1), 8);
}

TEST(CtcBackEndTest, ReturnToYard)
{
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);
    ctc.ManualDispatch(1, 0);
    ctc::Train train1;
    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.authority.size(), 171);

    std::vector<types::BlockId> route = ctc.GetRoute(70, 75);
    ASSERT_EQ(route.size(), 5);
}

TEST(CtcBackEndTest, ManualDispatchToBlock)
{
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);

    ASSERT_EQ(ctc.GetNumStation(), 18);
    ctc.ManualDispatch(1, 105);

    std::vector<ctc::Train> trains = ctc.GetTrains();
    ASSERT_EQ(trains.size(), 1);

    ctc::Train train1;
    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.authority.front(), 63);

    types::Error error = ctc.UpdateSuggestedSpeedAndAuthority(2);
    ASSERT_EQ(error, types::Error::ERROR_INVALID_TRAIN);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.authority.front(), 64);
    ASSERT_EQ(train1.suggested_speed, 19);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.authority.front(), 65);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.authority.front(), 66);

    ctc.UpdateSuggestedSpeedAndAuthority(1);
    ctc.GetTrainById(1, train1);
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
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);

    std::vector<types::BlockState> block_states;
    types::BlockState              block_state_1(63, true, false);
    types::BlockState              block_state_2(70, true, true);
    block_states.push_back(block_state_1);
    block_states.push_back(block_state_2);

    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

    ASSERT_EQ(ctc.GetBlockById(63).occupied, true);
    ASSERT_EQ(ctc.GetBlockById(70).occupied, false);
    ASSERT_EQ(ctc.GetBlockById(70).failed, true);

    block_states.emplace_back(151, true, false);
    types::Error error = ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);
    ASSERT_EQ(error, types::Error::ERROR_INVALID_BLOCK);
}

TEST(CtcBackEndTest, TrainReceiveBlockOccupancy)
{
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);

    ctc.ManualDispatch(1, 105);

    ctc::Train train1;
    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.authority.front(), 63);
    ASSERT_EQ(train1.current_position, 0);

    std::vector<types::BlockState> block_states;
    types::BlockState              block_state_1(63, true, false);
    block_states.push_back(block_state_1);
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.authority.front(), 64);
    ASSERT_EQ(ctc.GetTrainCurrentPosition(1), 63);
    ASSERT_EQ(ctc.GetTrainSuggestedSpeed(1), 19);
}

TEST(CtcBackEndTest, ManualDispatchToMultipleBlock)
{
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);
    ctc.ManualDispatch(1, 67);
    ctc.ManualDispatch(1, 70);
    ctc::Train train1;
    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.current_position, 0);      // Sitting in yard
    ASSERT_EQ(train1.authority.size(), 5);      // Authority = 5 blocks to get to block 67
    ASSERT_EQ(train1.destination_list[CTC_TRAIN_CURRENT_DESTINATION].destination, 67);
    ASSERT_EQ(train1.destination_list[1].destination, 70);

    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, { types::BlockState(63, true, false) });
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, { types::BlockState(64, true, false) });
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, { types::BlockState(65, true, false) });
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, { types::BlockState(66, true, false) });
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, { types::BlockState(67, true, false) });
    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.current_position, 67);
    ASSERT_EQ(train1.authority.size(), 3);
    ASSERT_EQ(train1.destination_list[CTC_TRAIN_CURRENT_DESTINATION].destination, 70);

    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, { types::BlockState(68, true, false) });
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, { types::BlockState(69, true, false) });
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, { types::BlockState(70, true, false) });
    ctc.GetTrainById(1, train1);
    ASSERT_EQ(train1.current_position, 70);
    ASSERT_EQ(train1.authority.size(), 163);
    ASSERT_EQ(train1.destination_list[CTC_TRAIN_CURRENT_DESTINATION].destination, 0);
}

TEST(CtcBackEndTest, GetSuggestedSpeedAndAuthorities)
{
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);
    // Dispatch train to block 70
    ctc.ManualDispatch(1, 70);

    // CTC Received Block Occupancy at 63
    std::vector<types::BlockState> block_states = { types::BlockState(63, true, false) };
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

    std::vector<types::TrackCircuitData> track_circuit_data = ctc.GetSuggestedSpeedsAndAuthorities();
    ASSERT_EQ(track_circuit_data.size(), 1);
    types::TrackCircuitData data = track_circuit_data[0];
    ASSERT_EQ(data.track, types::TrackId::TRACKID_GREEN);
    ASSERT_EQ(data.authority, 7);
    ASSERT_EQ(data.block, 63);
    ASSERT_EQ(data.speed, 19);

    // CTC Received Block Occupancy at 64
    block_states[0].block = 64;
    ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

    track_circuit_data = ctc.GetSuggestedSpeedsAndAuthorities();
    data               = track_circuit_data[0];
    ASSERT_EQ(data.track, types::TrackId::TRACKID_GREEN);
    ASSERT_EQ(data.authority, 6);
    ASSERT_EQ(data.block, 64);
    ASSERT_EQ(data.speed, 19);
}

static std::string TimePointToString(const std::chrono::system_clock::time_point& time_point)
{
    std::stringstream buffer;
    std::time_t       time_t_point = std::chrono::system_clock::to_time_t(time_point);
    std::tm           local_time   = *std::localtime(&time_t_point);
    buffer << std::put_time(&local_time, "%T");
    return buffer.str();
}