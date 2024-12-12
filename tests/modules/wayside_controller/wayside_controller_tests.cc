/*****************************************************************************
* @file wayside_controller_tests.cc
*
* @brief Unit testing for WaysideController.
*****************************************************************************/

#include <filesystem>
#include <ranges>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "controller_handler.h"
#include "green_line_blocks.h"
#include "simulator.h"
#include "track_model.h"
#include "wayside_controller.h"
#include "wayside_controller_handler.h"

class MockCtc : public ctc::Ctc
{
    public:
        MOCK_METHOD(types::Error, SetBlockStates, (const types::TrackId track, const std::vector<types::BlockState> &block_states), (override));
        MOCK_METHOD(std::vector<types::TrackCircuitData>, GetSuggestedSpeedsAndAuthorities, (), (const, override));
};

class MockTrackModel : public track_model::TrackModel
{
    public:
        MOCK_METHOD(types::TrackId, GetTrackId, (), (override));
        MOCK_METHOD(types::Error, AddTrainModel, (std::shared_ptr<train_model::TrainModel> train), (override));
        MOCK_METHOD(std::shared_ptr<train_model::TrainModel>, GetTrainModel, (const types::TrainId train), (const, override));
        MOCK_METHOD(void, GetTrainModels, (std::vector<std::shared_ptr<train_model::TrainModel>> & trains), (override));
        MOCK_METHOD(void, Update, (), (override));
        MOCK_METHOD(types::Error, SetSwitchState, (const types::BlockId block, const bool switched), (override));
        MOCK_METHOD(types::Error, SetCrossingState, (const types::BlockId block, const bool closed), (override));
        MOCK_METHOD(types::Error, SetRedTrafficLight, (const types::BlockId block, const bool on), (override));
        MOCK_METHOD(types::Error, SetYellowTrafficLight, (const types::BlockId block, const bool on), (override));
        MOCK_METHOD(types::Error, SetGreenTrafficLight, (const types::BlockId block, const bool on), (override));
        MOCK_METHOD(types::Error, SetCommandedSpeed, (const types::BlockId block, const types::MetersPerSecond speed), (override));
        MOCK_METHOD(types::Error, SetAuthority, (const types::BlockId block, const types::Blocks authority), (override));
        MOCK_METHOD(types::Error, GetBlockOccupancy, (const types::BlockId block, bool &occupied), (const, override));
};

static const std::array<bool, wayside_controller::kTotalInputs> kInputs = {true, true, false, false, true, false, false, false, false, false, false, false, true, false, true, false, false, true, false, false, true, true, true, true, false, false, false, true, false, true, false, true, false, true, false, false, true, false, false, false, false,
                                                                           false, false, false, true, true, false, false, true, true, false, false, false, false, true, true, false, true, true, false, true, false, false, true, true, true, false, false, false, false, false, true, false, true, false, true, false, false, false, true, false, true, false, false, false};

static const std::vector<wayside_controller::WaysideBlock> kBlueLineWaysideBlocks = {
    wayside_controller::WaysideBlock(1, 2, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 0, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(2, 3, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 1, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(3, 4, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 2, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(4, 5, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 3, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(5, 6, 6, 11, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 4, 78, 0, true, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(6, 7, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 5, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(7, 8, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 6, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(8, 9, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 7, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(9, 10, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 8, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(10, types::kEndBlock, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 9, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(11, 12, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 10, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(12, 13, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 11, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(13, 14, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 12, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(14, 15, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 13, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(15, 16, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 14, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW),
    wayside_controller::WaysideBlock(16, types::kEndBlock, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 15, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW)};

wayside_controller::Error GetInput(const wayside_controller::InputId input, wayside_controller::IoSignal &signal)
{
    wayside_controller::Error error = wayside_controller::Error::ERROR_INVALID_INPUT;

    if (input < kInputs.size())
    {
        signal = wayside_controller::IoSignal::IOSIGNAL_LOW;

        if (kInputs[input])
        {
            signal = wayside_controller::IoSignal::IOSIGNAL_HIGH;
        }

        error = wayside_controller::Error::ERROR_NONE;
    }

    return error;
}

wayside_controller::Error SetOutput(const wayside_controller::OutputId output, const wayside_controller::IoSignal signal)
{
    wayside_controller::Error error = wayside_controller::Error::ERROR_INVALID_OUTPUT;

    if (output < wayside_controller::kTotalOutputs)
    {
        // TODO NNF-105 set outputs
        (void)(signal); // temporary fix to remove compiler warnings
        error = wayside_controller::Error::ERROR_NONE;
    }

    return error;
}

TEST(WaysideControllerTests, Configure)
{
    std::vector<wayside_controller::WaysideBlock> blocks = kBlueLineWaysideBlocks;
    wayside_controller::WaysideController         software_wayside_controller(GetInput);

    // Success
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.Configure(blocks));

    // Empty configuration
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.Configure(std::vector<wayside_controller::WaysideBlock>()));

    // Duplicate block ID
    blocks.push_back(wayside_controller::WaysideBlock(1, 2, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 16, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW));
    ASSERT_EQ(wayside_controller::Error::ERROR_DUPLICATE_BLOCK, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Duplicate block ID
    blocks.push_back(wayside_controller::WaysideBlock(1, 3, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 16, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW));
    ASSERT_EQ(wayside_controller::Error::ERROR_DUPLICATE_BLOCK, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid track circuit input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 78, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW));
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid track circuit input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, -1, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW));
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Duplicate track circuit input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 0, 0, 0, false, false, wayside_controller::IoSignal::IOSIGNAL_LOW));
    ASSERT_EQ(wayside_controller::Error::ERROR_DUPLICATE_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid switch input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 16, 77, 0, true, false, wayside_controller::IoSignal::IOSIGNAL_LOW));
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid switch input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 16, wayside_controller::kTotalInputs, 1, true, false, wayside_controller::IoSignal::IOSIGNAL_LOW));
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid switch input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 16, -1, 2, true, false, wayside_controller::IoSignal::IOSIGNAL_LOW));
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Duplicate switch input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, 0, types::BlockDirection::BLOCKDIRECTION_UNIDIRECTIONAL, 16, 78, 3, true, false, wayside_controller::IoSignal::IOSIGNAL_LOW));
    ASSERT_EQ(wayside_controller::Error::ERROR_DUPLICATE_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();
}

TEST(WaysideControllerTests, GetCommandedSpeedAndAuthority)
{
    // TODO NNF-144 test commanded speed

    std::array<wayside_controller::IoSignal, 16>                                                                            inputs     = {wayside_controller::IoSignal::IOSIGNAL_LOW};
    std::function<wayside_controller::Error(const wayside_controller::InputId input, wayside_controller::IoSignal &signal)> get_inputs =
        [&inputs](const wayside_controller::InputId input, wayside_controller::IoSignal &signal)
        {
            wayside_controller::Error error = wayside_controller::Error::ERROR_INVALID_INPUT;

            if (input < inputs.size())
            {
                signal = inputs[input];

                error = wayside_controller::Error::ERROR_NONE;
            }

            return error;
        };

    types::TrackCircuitData               track_circuit_data;
    wayside_controller::WaysideController software_wayside_controller(get_inputs, kBlueLineWaysideBlocks);

    // Valid authority
    track_circuit_data.block     = 1;
    track_circuit_data.speed     = 0;
    track_circuit_data.authority = 0;
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(0, track_circuit_data.authority);

    // Valid authority, does not exceed given authority
    track_circuit_data.authority = 9;
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(9, track_circuit_data.authority);

    // Valid authority, does not exceed given authority
    track_circuit_data.authority = 5;
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(5, track_circuit_data.authority);

    // Valid authority to end of track
    track_circuit_data.authority = 10;
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(10, track_circuit_data.authority);

    // Sets safe authority from invalid authority
    track_circuit_data.authority = 16;
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(10, track_circuit_data.authority);

    // Sets safe authority based on occupancies
    track_circuit_data.authority = 10;
    inputs[4]                    = wayside_controller::IoSignal::IOSIGNAL_HIGH;
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(3, track_circuit_data.authority);
    inputs[4] = wayside_controller::IoSignal::IOSIGNAL_LOW;

    // Invalid block
    track_circuit_data.block = -1;
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_BLOCK, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(0, track_circuit_data.authority);

    // Invalid block
    track_circuit_data.block = 0;
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_BLOCK, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(0, track_circuit_data.authority);

    // Invalid block
    track_circuit_data.block = 17;
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_BLOCK, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(0, track_circuit_data.authority);

    // Force get input error
    std::function<wayside_controller::Error(const wayside_controller::InputId input, wayside_controller::IoSignal &signal)> get_inputs_none =
        [](const wayside_controller::InputId input, wayside_controller::IoSignal &signal)
        {
            (void)(input);
            (void)(signal);
            return wayside_controller::Error::ERROR_INVALID_INPUT;
        };
    wayside_controller::WaysideController software_wayside_controlle_no_inputs(get_inputs_none, kBlueLineWaysideBlocks);
    track_circuit_data.block     = 1;
    track_circuit_data.authority = 5;
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_INPUT, software_wayside_controlle_no_inputs.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(0, track_circuit_data.authority);

    // TODO NNF-105 test authority based on switch position
}

TEST(WaysideControllerTests, SetMaintenanceMode)
{
    // TODO NNF-105
}

TEST(WaysideControllerTests, SetSwitch)
{
    // TODO NNF-105

    wayside_controller::WaysideController software_wayside_controller(GetInput, kBlueLineWaysideBlocks);

    // Valid switch
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.SetSwitch(5, true));
    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.SetSwitch(5, false));

    // Invalid block
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_BLOCK, software_wayside_controller.SetSwitch(17, true));

    // Block does not have switch
    ASSERT_EQ(wayside_controller::Error::ERROR_INVALID_BLOCK, software_wayside_controller.SetSwitch(1, true));
}

TEST(WaysideControllerTests, GetBlockStates)
{
    wayside_controller::WaysideController software_wayside_controller(GetInput, kBlueLineWaysideBlocks);
    std::vector<types::BlockState>        block_states;

    ASSERT_EQ(wayside_controller::Error::ERROR_NONE, software_wayside_controller.GetBlockStates(block_states));
    ASSERT_EQ(5, block_states.size());

    // Block 1 occupied
    ASSERT_NE(block_states.end(), std::ranges::find_if(block_states, [](const types::BlockState block_state)
    {
        return ((block_state.block == 1) && block_state.occupied);
    }));

    // Block 2 occupied
    ASSERT_NE(block_states.end(), std::ranges::find_if(block_states, [](const types::BlockState block_state)
    {
        return ((block_state.block == 2) && block_state.occupied);
    }));

    // Block 5 occupied
    ASSERT_NE(block_states.end(), std::ranges::find_if(block_states, [](const types::BlockState block_state)
    {
        return ((block_state.block == 5) && block_state.occupied);
    }));

    // Block 13 occupied
    ASSERT_NE(block_states.end(), std::ranges::find_if(block_states, [](const types::BlockState block_state)
    {
        return ((block_state.block == 13) && block_state.occupied);
    }));

    // Block 15 occupied
    ASSERT_NE(block_states.end(), std::ranges::find_if(block_states, [](const types::BlockState block_state)
    {
        return ((block_state.block == 15) && block_state.occupied);
    }));
}

TEST(WaysideControllerTests, TrackCircuitDataEndToEnd)
{
    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::Return;
    using ::testing::SetArgReferee;

    MockCtc                                                    ctc_mock;
    simulator::Simulator                                       world;
    CsvParser                                                  csv_parser(std::filesystem::current_path() / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv");
    BlockBuilder                                               block_builder(csv_parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
    RingBuffer<uint8_t, 1024>                                  buffer_0, buffer_1;
    wayside_controller::SoftwareWaysideControllerHandler<1024> wayside_controller_handler(1,
                                                                                          types::TrackId::TRACKID_GREEN,
                                                                                          wayside_controller::kGreenLineBlocksWayside1,
                                                                                          controller_network::BuildSoftwareBasicControllerPort<1024>(buffer_0, buffer_1));
    controller_network::ControllerHandler<1024> controller_handler;
    controller_handler.AddPort(controller_network::BuildSoftwareBasicControllerPort<1024>(buffer_1, buffer_0));
    controller_handler.SetWaysideLayout(block_builder.GetBlocks());

    // Add track model mock to simulation
    std::shared_ptr<MockTrackModel> mock_track = std::make_shared<MockTrackModel>();
    EXPECT_CALL(*mock_track.get(), GetTrackId()).Times(1).WillOnce(Return(types::TrackId::TRACKID_GREEN));
    world.AddTrackModel(mock_track);

    // Connect wayside controller
    ASSERT_EQ(types::Error::ERROR_NONE, wayside_controller_handler.Connect());
    EXPECT_CALL(ctc_mock, GetSuggestedSpeedsAndAuthorities()).Times(1);
    controller_handler.Update(ctc_mock, world);
    ASSERT_TRUE(controller_handler.IsControllerConnected(controller_network::CONTROLLERTYPE_WAYSIDE, 1));

    // Send track circuit data to wayside controller
    std::vector<types::TrackCircuitData> track_circuit_data = {
        {types::TrackId::TRACKID_GREEN, 70, 10, 20}};
    EXPECT_CALL(ctc_mock, GetSuggestedSpeedsAndAuthorities()).Times(1).WillOnce(Return(track_circuit_data));
    EXPECT_CALL(*mock_track.get(), GetBlockOccupancy(_, _)).Times(wayside_controller::kGreenLineBlocksWayside1.size()).WillRepeatedly(DoAll(SetArgReferee<1>(false), Return(types::Error::ERROR_NONE)));
    controller_handler.Update(ctc_mock, world);

    // Wayside controller receives track circuit data and sends back to track model
    wayside_controller_handler.Update();

    // Track model mock receives track circuit data
    EXPECT_CALL(ctc_mock, GetSuggestedSpeedsAndAuthorities()).Times(1).WillOnce(Return(track_circuit_data));
    EXPECT_CALL(*mock_track.get(), GetBlockOccupancy(_, _)).Times(wayside_controller::kGreenLineBlocksWayside1.size()).WillRepeatedly(DoAll(SetArgReferee<1>(false), Return(types::Error::ERROR_NONE)));
    // TODO NNF-144 test clamping to safe speed
    EXPECT_CALL(*mock_track.get(), SetCommandedSpeed(70, 10)).Times(1).WillOnce(Return(types::Error::ERROR_NONE));
    EXPECT_CALL(*mock_track.get(), SetAuthority(70, 20)).Times(1).WillOnce(Return(types::Error::ERROR_NONE));
    controller_handler.Update(ctc_mock, world);
}

TEST(WaysideControllerTests, BlockStatesEndToEnd)
{
    using ::testing::_;
    using ::testing::DoAll;
    using ::testing::Return;
    using ::testing::SetArgReferee;

    MockCtc                                                    ctc_mock;
    simulator::Simulator                                       world;
    CsvParser                                                  csv_parser(std::filesystem::current_path() / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv");
    BlockBuilder                                               block_builder(csv_parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
    RingBuffer<uint8_t, 1024>                                  buffer_0, buffer_1;
    wayside_controller::SoftwareWaysideControllerHandler<1024> wayside_controller_handler(1,
                                                                                          types::TrackId::TRACKID_GREEN,
                                                                                          wayside_controller::kGreenLineBlocksWayside1,
                                                                                          controller_network::BuildSoftwareBasicControllerPort<1024>(buffer_0, buffer_1));
    controller_network::ControllerHandler<1024> controller_handler;
    controller_handler.AddPort(controller_network::BuildSoftwareBasicControllerPort<1024>(buffer_1, buffer_0));
    controller_handler.SetWaysideLayout(block_builder.GetBlocks());

    // Add track model mock to simulation
    std::shared_ptr<MockTrackModel> mock_track = std::make_shared<MockTrackModel>();
    EXPECT_CALL(*mock_track.get(), GetTrackId()).Times(1).WillOnce(Return(types::TrackId::TRACKID_GREEN));
    world.AddTrackModel(mock_track);

    // Connect wayside controller
    ASSERT_EQ(types::Error::ERROR_NONE, wayside_controller_handler.Connect());
    EXPECT_CALL(ctc_mock, GetSuggestedSpeedsAndAuthorities()).Times(1);
    controller_handler.Update(ctc_mock, world);
    ASSERT_TRUE(controller_handler.IsControllerConnected(controller_network::CONTROLLERTYPE_WAYSIDE, 1));

    // Send block occupancies from the track model to the wayside controller
    EXPECT_CALL(ctc_mock, GetSuggestedSpeedsAndAuthorities()).Times(1);
    EXPECT_CALL(*mock_track.get(), GetBlockOccupancy(_, _)).Times(wayside_controller::kGreenLineBlocksWayside1.size()).WillRepeatedly(DoAll(SetArgReferee<1>(true), Return(types::Error::ERROR_NONE)));
    controller_handler.Update(ctc_mock, world);

    // Wayside controller receives block occupancies and send block states to CTC
    wayside_controller_handler.Update();

    // CTC receives block states
    EXPECT_CALL(ctc_mock, SetBlockStates(types::TrackId::TRACKID_GREEN, _)).Times(1);
    EXPECT_CALL(ctc_mock, GetSuggestedSpeedsAndAuthorities()).Times(1);
    EXPECT_CALL(*mock_track.get(), GetBlockOccupancy(_, _)).Times(wayside_controller::kGreenLineBlocksWayside1.size()).WillRepeatedly(DoAll(SetArgReferee<1>(false), Return(types::Error::ERROR_NONE)));
    controller_handler.Update(ctc_mock, world);
}