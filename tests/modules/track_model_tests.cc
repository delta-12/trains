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
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser                       parser(path);
    BlockBuilder                    bb(parser.GetRecords());
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;
    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks());

    bool occupancy1;
    ASSERT_EQ(track.GetBlockOccupancy(2, occupancy1), types::ERROR_NONE);
    ASSERT_EQ(occupancy1, 0);
    ASSERT_EQ(track.SetTrackCircuitFailure(2, 1), types::ERROR_NONE);
    bool occupancy;
    ASSERT_EQ(track.GetBlockOccupancy(2, occupancy), types::ERROR_NONE);
    ASSERT_EQ(occupancy, 1);

    ASSERT_EQ(bb.GetSize(), 151);

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
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    CsvParser                       parser(path);
    BlockBuilder                    bb(parser.GetRecords());
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;

    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks());
    Graph<types::BlockId, types::Meters> graph;
    auto                                 bbtemp = bb.GetBlocks().size();

    for (int i = 0; i < bbtemp; i++)
    {
        // every edge is really the length of the second block
        graph.AddEdge(i, i + 1, bb.GetBlocks()[i + 1].length);
    }

    ASSERT_EQ(graph.BreadthFirstSearch(0).size(), 152);

    std::shared_ptr<train_model::TrainModel> ptr = std::make_shared<train_model::TrainModelImpl>(train);
    track.AddTrainModel(ptr);

    std::vector<std::shared_ptr<train_model::TrainModel>> trains;

    track.GetTrainModels(trains);

    ASSERT_EQ(trains.size(), 1);

    //update
    track.Update();

    // Set authority and speed
    ASSERT_EQ(track.SetAuthority(2, 5), types::ERROR_NONE);
    ASSERT_EQ(track.SetCommandedSpeed(2, 50), types::ERROR_NONE);

    //check that authority is set
    ASSERT_EQ(ptr->GetAuthority(), 5);
    //check speed is set
    ASSERT_EQ(ptr->GetCommandedSpeed(), 50);

    //check passenger count
    ASSERT_NE(ptr->GetPassengersDeboarding(), 0);

    //UPDATE 2
    track.Update();

    // Set authority and speed
    ASSERT_EQ(track.SetAuthority(4, 8), types::ERROR_NONE);
    ASSERT_EQ(track.SetCommandedSpeed(4, 90), types::ERROR_NONE);

    //check that authority is set
    ASSERT_EQ(ptr->GetAuthority(), 8);
    //check speed is set
    ASSERT_EQ(ptr->GetCommandedSpeed(), 90);

}