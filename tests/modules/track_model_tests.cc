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

namespace train_model
{

// Definition of the TrainModelImpl class
class TrainModelImpl : public TrainModel
{
    public:
        void SetTrainId(const types::TrainId train);
        types::TrainId GetTrainId(void) const;
        void SetEmergencyBrake(const bool emergency_brake);
        void SetEngineFailure(const bool engine_failure);
        void SetBrakeFailure(const bool brake_failure);
        void SetSignalPickupFailure(const bool signal_pickup_failure);
        bool GetBrakeFailure(void) const;
        std::size_t GetBeaconData(types::BeaconData &data) const;
        types::Blocks GetAuthority(void) const;
        types::MetersPerSecond GetCommandedSpeed(void) const;
        types::MetersPerSecond GetActualSpeed(void) const;
        bool GetEngineFailure(void) const;
        bool GetSignalPickupFailure(void) const;
        types::Watts GetActualPower(void) const;
        types::DegreesFahrenheit GetActualInternalTemperature(void) const;
        types::Polarity GetTrackPolarity(void) const;
        void SetStationAnnouncement(const std::string &announcement);
        void SetHeadlights(const bool on);
        void SetInternalLights(const bool on);
        void SetLeftDoorsState(const bool open);
        void SetRightDoorsState(const bool open);
        void SetCommandedPower(const types::Watts power);
        void SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees);
        void SetBlockId(const types::BlockId block);
        uint16_t GetPassengersDeboarding(void);
        types::Meters GetDistanceTraveled(void) const;
        void SetCommandedSpeed(const types::MetersPerSecond speed);
        void SetAuthority(const types::Blocks blocks);
        void SetPassengersBoarding(const uint16_t passengers);
        void SetTrackPolarity(const types::Polarity polarity);
        void SetBeaconData(const types::BeaconData &data, std::size_t &size);
        void SetDistanceTraveled(const types::Meters distance);
        void SetGrade(const double grade);
        void SetBrake(const double brake);

    private:
        types::TrainId train_id_;
        bool emergency_brake_;
        bool engine_failure_;
        bool brake_failure_;
        bool signal_pickup_failure_;
        types::Meters commanded_speed_;
        types::MetersPerSecond actual_speed_;
        types::Blocks authority_;
        uint16_t current_passengers_ = 0;
        bool left_doors_open_;
        bool right_doors_open_;
        types::DegreesFahrenheit internal_temperature_;
        types::Watts commanded_power_;
        types::Watts actual_power_;
        types::Polarity track_polarity_;
        std::string announcement_;
        bool headlights_on_;
        bool internal_lights_on_;
        types::BlockId block_id_;
        double grade_;
        double brake_;
};

void TrainModelImpl::SetGrade(const double grade)
{
    grade_ = grade;
}

void TrainModelImpl::SetBrake(const double brake)
{
    brake_ = brake;
}

void TrainModelImpl::SetTrainId(const types::TrainId train)
{
    train_id_ = train;
}

types::TrainId TrainModelImpl::GetTrainId(void) const
{
    return train_id_;
}

void TrainModelImpl::SetEmergencyBrake(const bool emergency_brake)
{
    emergency_brake_ = emergency_brake;
}

void TrainModelImpl::SetEngineFailure(const bool engine_failure)
{
    engine_failure_ = engine_failure;
}

void TrainModelImpl::SetBrakeFailure(const bool brake_failure)
{
    brake_failure_ = brake_failure;
}

void TrainModelImpl::SetSignalPickupFailure(const bool signal_pickup_failure)
{
    signal_pickup_failure_ = signal_pickup_failure;
}

bool TrainModelImpl::GetBrakeFailure(void) const
{
    return brake_failure_;
}

std::size_t TrainModelImpl::GetBeaconData(types::BeaconData &data) const
{
    // Stub for getting beacon data, return size of data
    return sizeof(data);
}

types::Blocks TrainModelImpl::GetAuthority(void) const
{
    return authority_;
}

types::MetersPerSecond TrainModelImpl::GetCommandedSpeed(void) const
{
    return commanded_speed_;
}

types::MetersPerSecond TrainModelImpl::GetActualSpeed(void) const
{
    return actual_speed_;
}

bool TrainModelImpl::GetEngineFailure(void) const
{
    return engine_failure_;
}

bool TrainModelImpl::GetSignalPickupFailure(void) const
{
    return signal_pickup_failure_;
}

types::Watts TrainModelImpl::GetActualPower(void) const
{
    return actual_power_;
}

types::DegreesFahrenheit TrainModelImpl::GetActualInternalTemperature(void) const
{
    return internal_temperature_;
}

types::Polarity TrainModelImpl::GetTrackPolarity(void) const
{
    return track_polarity_;
}

void TrainModelImpl::SetDistanceTraveled(const types::Meters distance)
{
}

void TrainModelImpl::SetStationAnnouncement(const std::string &announcement)
{
    // Handle station announcement
    announcement_ = announcement;
}

void TrainModelImpl::SetHeadlights(const bool on)
{
    headlights_on_ = on;
}

void TrainModelImpl::SetInternalLights(const bool on)
{
    internal_lights_on_ = on;
}

void TrainModelImpl::SetLeftDoorsState(const bool open)
{
    left_doors_open_ = open;
}

void TrainModelImpl::SetRightDoorsState(const bool open)
{
    right_doors_open_ = open;
}

void TrainModelImpl::SetCommandedPower(const types::Watts power)
{
    commanded_power_ = power;
}

void TrainModelImpl::SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees)
{
    internal_temperature_ = degrees;
}

void TrainModelImpl::SetBlockId(const types::BlockId block)
{
    block_id_ = block;
}

uint16_t TrainModelImpl::GetPassengersDeboarding(void)
{
    if (current_passengers_ < 5)
    {
        return 0;
    }
    else
    {
        current_passengers_ -= 5;
        return 5;
    }
}

types::Meters TrainModelImpl::GetDistanceTraveled(void) const
{
    return 110;
}

void TrainModelImpl::SetCommandedSpeed(const types::MetersPerSecond speed)
{
    commanded_speed_ = speed;
}

void TrainModelImpl::SetAuthority(const types::Blocks blocks)
{
    authority_ = blocks;
}

void TrainModelImpl::SetPassengersBoarding(const uint16_t passengers)
{
    current_passengers_ += passengers;
}

void TrainModelImpl::SetTrackPolarity(const types::Polarity polarity)
{
    track_polarity_ = polarity;
}

void TrainModelImpl::SetBeaconData(const types::BeaconData &data, std::size_t &size)
{
    // Stub for setting beacon data
    size = sizeof(data);
}

} // namespace train_model

TEST(TrackModelTests, GreenLine)
{
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
    std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;
    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

    bool occupancy1;

    ASSERT_EQ(track.GetTrackId(), types::TRACKID_GREEN);

    ASSERT_EQ(bb2.GetSize(), 151);

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

TEST(TrackModelTests, TrainSpeedAuthority)
{
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
    std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;

    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

    std::shared_ptr<train_model::TrainModel> ptr = std::make_shared<train_model::TrainModelImpl>(train);
    track.AddTrainModel(ptr);

    std::vector<std::shared_ptr<train_model::TrainModel>> trains;

    track.GetTrainModels(trains);

    ASSERT_EQ(trains.size(), 1);

    // update
    track.Update();

    // fake authority and speed
    ASSERT_EQ(track.SetAuthority(200, 5), types::ERROR_INVALID_BLOCK);
    ASSERT_EQ(track.SetCommandedSpeed(200, 50), types::ERROR_INVALID_BLOCK);
    ASSERT_EQ(track.SetAuthority(-1, 5), types::ERROR_INVALID_BLOCK);
    ASSERT_EQ(track.SetCommandedSpeed(-1, 50), types::ERROR_INVALID_BLOCK);

    // Set authority and speed
    ASSERT_EQ(track.SetAuthority(63, 5), types::ERROR_NONE);
    ASSERT_EQ(track.SetCommandedSpeed(63, 50), types::ERROR_NONE);

    // check that authority is set
    ASSERT_EQ(ptr->GetAuthority(), 5);
    // check speed is set
    ASSERT_EQ(ptr->GetCommandedSpeed(), 50);

    // check passenger count
    ASSERT_NE(ptr->GetPassengersDeboarding(), 0);

    // occupancy check
    bool occupied;
    ASSERT_EQ(track.GetBlockOccupancy(190, occupied), types::ERROR_INVALID_BLOCK);
    ASSERT_EQ(track.GetBlockOccupancy(-1, occupied), types::ERROR_INVALID_BLOCK);

    ASSERT_EQ(track.GetBlockOccupancy(63, occupied), types::ERROR_NONE);
    ASSERT_EQ(occupied, 1);

    ASSERT_EQ(track.GetBlockOccupancy(64, occupied), types::ERROR_NONE);
    ASSERT_EQ(occupied, 1);

    // UPDATE 2
    track.Update();

    // Set authority and speed
    ASSERT_EQ(track.SetAuthority(65, 8), types::ERROR_NONE);
    ASSERT_EQ(track.SetCommandedSpeed(65, 90), types::ERROR_NONE);

    // check that authority is set
    ASSERT_EQ(ptr->GetAuthority(), 8);
    // check speed is set
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

    // update 3 (longer)
    for (int i = 0; i < 36; i++)
    {
        track.Update();
    }

    auto otb = track.GetOccupiedTrainBlocks();

    // update 4 (takes u to the end)
    for (int i = 0; i < 145; i++)
    {
        track.Update();
    }

    // occupancy check when we loop back around
    ASSERT_EQ(track.GetBlockOccupancy(63, occupied), types::ERROR_NONE);
    ASSERT_EQ(occupied, 1);

    ASSERT_EQ(track.GetBlockOccupancy(64, occupied), types::ERROR_NONE);
    ASSERT_EQ(occupied, 1);

    otb = track.GetOccupiedTrainBlocks();

    // switching so we go back to the yard instead of looping around
    ASSERT_EQ(track.SetSwitchState(57, 1), types::ERROR_NONE);

    ASSERT_EQ(track.SetSwitchState(58, 1), types::ERROR_INVALID_BLOCK);

    track.Update();
    track.Update();

    for (int i = 0; i < 36; i++)
    {
        track.Update();
    }

    otb = track.GetOccupiedTrainBlocks();

    // update 4 (longer)
    for (int i = 0; i < 145; i++)
    {
        track.Update();
    }

    otb = track.GetOccupiedTrainBlocks();

    ASSERT_EQ(otb.size(), 0);

    std::vector<std::shared_ptr<train_model::TrainModel>> trainsempty;

    // make sure the train no longer exists
    track.GetTrainModels(trainsempty);
    ASSERT_EQ(trainsempty.size(), 0);
}

TEST(TrackModelTests, Switching)
{
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
    std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;

    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

    std::shared_ptr<train_model::TrainModel> ptr = std::make_shared<train_model::TrainModelImpl>(train);

    bool         occupancy;
    types::Block test_block;

    ASSERT_EQ(track.GetBlock(100, test_block), types::ERROR_NONE);

    ASSERT_EQ(test_block.switched, 0);

    ASSERT_EQ(track.SetSwitchState(100, 1), types::ERROR_NONE);

    ASSERT_EQ(track.GetBlock(100, test_block), types::ERROR_NONE);

    ASSERT_EQ(test_block.switched, 1);
}

TEST(TrackModelTests, Boarding)
{
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
    std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;

    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

    std::shared_ptr<train_model::TrainModel> ptr = std::make_shared<train_model::TrainModelImpl>(train);

    track.AddTrainModel(ptr);

    std::vector<std::shared_ptr<train_model::TrainModel>> trains;

    track.GetTrainModels(trains);

    ASSERT_EQ(trains.size(), 1);

    track.Update();
    track.Update();

    // train is now on block 3, where there is a station
    types::Block test_block;
    ASSERT_EQ(track.GetBlock(65, test_block), types::ERROR_NONE);

    ASSERT_EQ(test_block.has_station, 1);

    ASSERT_NE(ptr->GetPassengersDeboarding(), 0);

    ASSERT_EQ(track.RemoveTrainModel(2), types::ERROR_INVALID_TRAIN);
    ASSERT_EQ(track.RemoveTrainModel(0), types::ERROR_NONE);
}

TEST(TrackModelTests, Polarity)
{
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
    std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;

    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

    std::shared_ptr<train_model::TrainModel> ptr = std::make_shared<train_model::TrainModelImpl>(train);

    track.AddTrainModel(ptr);

    std::vector<std::shared_ptr<train_model::TrainModel>> trains;

    track.GetTrainModels(trains);

    ASSERT_EQ(trains.size(), 1);

    types::Polarity current_polarity;

    track.Update();

    current_polarity = ptr->GetTrackPolarity();

    ASSERT_EQ(current_polarity, types::POLARITY_NEGATIVE);

    track.Update();

    // train is now on block 65, where polarity is 1
    types::Block test_block;

    ASSERT_EQ(track.GetBlock(-1, test_block), types::ERROR_INVALID_BLOCK);
    ASSERT_EQ(track.GetBlock(200, test_block), types::ERROR_INVALID_BLOCK);
    ASSERT_EQ(track.GetBlock(65, test_block), types::ERROR_NONE);

    ASSERT_EQ(test_block.has_station, 1);

    current_polarity = ptr->GetTrackPolarity();

    ASSERT_EQ(current_polarity, types::POLARITY_POSITIVE);

    ASSERT_NE(ptr->GetPassengersDeboarding(), 0);
}

TEST(TrackModelTests, PlaceHolderFunctions)
{
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
    std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    types::Block                    block;
    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;
    track.SetTrackLayout(types::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

    bool occupancy1;

    ASSERT_EQ(track.GetTrackId(), types::TRACKID_GREEN);

    ASSERT_EQ(bb2.GetSize(), 151);

    ASSERT_EQ(track.SetCrossingState(5, 1), types::ERROR_NONE);
    ASSERT_EQ(track.SetRedTrafficLight(5, 1), types::ERROR_NONE);
    ASSERT_EQ(track.SetYellowTrafficLight(5, 1), types::ERROR_NONE);
    ASSERT_EQ(track.SetGreenTrafficLight(5, 1), types::ERROR_NONE);
    ASSERT_EQ(track.SetBrokenRail(5, 1), types::ERROR_NONE);
    ASSERT_EQ(track.SetTrackCircuitFailure(5, 1), types::ERROR_NONE);
    ASSERT_EQ(track.SetPowerFailure(5, 1), types::ERROR_NONE);
    ASSERT_EQ(track.SetExternalTemperature(60), types::ERROR_NONE);
}