/*****************************************************************************
* @file track_model_tests.cc
*
* @brief Unit testing for track model.
*****************************************************************************/

#include <gtest/gtest.h>
#include <memory>

#include "csv_parser.h"
#include "block_builder.h"
#include "train_model.h"
#include "types.h"
#include "track_model.h"

TEST(TrackModelTests, GreenLine)
{
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_layout.csv";
    std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords());
    BlockBuilder                    bb2(parser2.GetRecords());
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;
    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

    bool occupancy1;
    ASSERT_EQ(track.GetBlockOccupancy(2, occupancy1), types::ERROR_NONE);
    ASSERT_EQ(occupancy1, 0);
    ASSERT_EQ(track.SetTrackCircuitFailure(2, 1), types::ERROR_NONE);
    bool occupancy;
    ASSERT_EQ(track.GetBlockOccupancy(2, occupancy), types::ERROR_NONE);
    ASSERT_EQ(occupancy, 1);

    ASSERT_EQ(bb2.GetSize(), 151);

    ASSERT_EQ(types::ERROR_NONE, bb.GetBlock(1, block));
    ASSERT_EQ(block.has_crossing, false);
    ASSERT_EQ(block.has_switch, true);
    //ASSERT_EQ(block.direction, types::BLOCKDIRECTION_UNIDIRECTIONAL);
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

TEST(TrackModelTests, TrainSpeedAuthority)
{
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_layout.csv";
    std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords());
    BlockBuilder                    bb2(parser2.GetRecords());
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;

    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());
    Graph<types::BlockId, types::Meters> graph;

    std::shared_ptr<train_model::TrainModel> ptr = std::make_shared<train_model::TrainModelImpl>(train);
    track.AddTrainModel(ptr);

    std::vector<std::shared_ptr<train_model::TrainModel>> trains;

    track.GetTrainModels(trains);

    ASSERT_EQ(trains.size(), 1);

    //update
    track.Update();

    // Set authority and speed
    ASSERT_EQ(track.SetAuthority(63, 5), types::ERROR_NONE);
    ASSERT_EQ(track.SetCommandedSpeed(63, 50), types::ERROR_NONE);

    //check that authority is set
    ASSERT_EQ(ptr->GetAuthority(), 5);
    //check speed is set
    ASSERT_EQ(ptr->GetCommandedSpeed(), 50);

    //check passenger count
    ASSERT_EQ(ptr->GetPassengersDeboarding(), 0);

    //occupancy check
    bool occupied;
    ASSERT_EQ(track.GetBlockOccupancy(63, occupied), types::ERROR_NONE);
    ASSERT_EQ(occupied, 1);

    ASSERT_EQ(track.GetBlockOccupancy(64, occupied), types::ERROR_NONE);
    ASSERT_EQ(occupied, 1);

    //UPDATE 2
    track.Update();

    // Set authority and speed
    ASSERT_EQ(track.SetAuthority(65, 8), types::ERROR_NONE);
    ASSERT_EQ(track.SetCommandedSpeed(65, 90), types::ERROR_NONE);

    //check that authority is set
    ASSERT_EQ(ptr->GetAuthority(), 8);
    //check speed is set
    ASSERT_EQ(ptr->GetCommandedSpeed(), 90);

    bool occupancy65;
    bool occupancy64;
    bool occupancy63;
    track.GetBlockOccupancy(65, occupancy65);
    track.GetBlockOccupancy(64, occupancy64);
    track.GetBlockOccupancy(63, occupancy63);
    ASSERT_EQ(occupancy64, 1);
    ASSERT_EQ(occupancy65, 1);
    ASSERT_EQ(occupancy63, 0);

    //update 3 (longer)
    for (int i=0;i<36;i++)
    {
        track.Update();
    }

    auto otb=track.GetOccupiedTrainBlocks();

    // auto otb = track.GetOccupiedTrainBlocks();
    for (int i=0;i<otb[0].size();i++)
    {
        std::cout << std::endl << otb[0][i] << std::endl;
    }

    //update 4 (takes u to the end)
    for (int i=0;i<145;i++)
    {
        track.Update();
    }

    //occupancy check when we loop back around
    ASSERT_EQ(track.GetBlockOccupancy(63, occupied), types::ERROR_NONE);
    ASSERT_EQ(occupied, 1);

    ASSERT_EQ(track.GetBlockOccupancy(64, occupied), types::ERROR_NONE);
    ASSERT_EQ(occupied, 1);

    otb=track.GetOccupiedTrainBlocks();

    //switching so we go back to the yard instead of looping around
    track.SetSwitchState(57, 1);

    track.Update();
    track.Update();
    for (int i=0;i<36;i++)
    {
        track.Update();
    }

    otb=track.GetOccupiedTrainBlocks();

    // auto otb = track.GetOccupiedTrainBlocks();
    // for (int i=0;i<otb[0].size();i++)
    // {
    //     std::cout << std::endl << otb[0][i] << std::endl;
    // }

    //update 4 (longer)
    for (int i=0;i<145;i++)
    {
        track.Update();
    }

    otb=track.GetOccupiedTrainBlocks();

    // auto otb = track.GetOccupiedTrainBlocks();
    for (int i=0;i<otb[0].size();i++)
    {
        std::cout << std::endl << otb[0][i] << std::endl;
    }

    std::vector<std::shared_ptr<train_model::TrainModel>> trainsempty;

    //make sure the train no longer exists
    track.GetTrainModels(trainsempty);
    ASSERT_EQ(trainsempty.size(), 0);
}