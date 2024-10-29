/*****************************************************************************
* @file wayside_controller_tests.cc
*
* @brief Unit testing for WaysideController.
*****************************************************************************/

#include <gtest/gtest.h>

#include "logger.h"
#include "wayside_controller.h"

static const std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> kInputs = {true, true, false, false, true, false, false, false, false, false, false, false, true, false, true, false, false, true, false, false, true, true, true, true, false, false, false, true, false, true, false, true, false, true, false, false, true, false, false, false, false,
                                                                               false, false, false, true, true, false, false, true, true, false, false, false, false, true, true, false, true, true, false, true, false, false, true, true, true, false, false, false, false, false, true, false, true, false, true, false, false, false, true, false, true, false, false, false};

static const std::vector<wayside_controller::WaysideBlock> kBlueLineWaysideBlocks = {
    wayside_controller::WaysideBlock(1, 2, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 0, 0, false, false, false),
    wayside_controller::WaysideBlock(2, 3, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 1, 0, false, false, false),
    wayside_controller::WaysideBlock(3, 4, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 2, 0, false, false, false),
    wayside_controller::WaysideBlock(4, 5, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 3, 0, false, false, false),
    wayside_controller::WaysideBlock(5, 6, 11, types::BLOCKDIRECTION_UNIDIRECTIONAL, 4, 78, true, false, false),
    wayside_controller::WaysideBlock(6, 7, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 5, 0, false, false, false),
    wayside_controller::WaysideBlock(7, 8, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 6, 0, false, false, false),
    wayside_controller::WaysideBlock(8, 9, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 7, 0, false, false, false),
    wayside_controller::WaysideBlock(9, 10, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 8, 0, false, false, false),
    wayside_controller::WaysideBlock(10, 0, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 9, 0, false, false, false),
    wayside_controller::WaysideBlock(11, 12, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 10, 0, false, false, false),
    wayside_controller::WaysideBlock(12, 13, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 11, 0, false, false, false),
    wayside_controller::WaysideBlock(13, 14, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 12, 0, false, false, false),
    wayside_controller::WaysideBlock(14, 15, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 13, 0, false, false, false),
    wayside_controller::WaysideBlock(15, 16, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 14, 0, false, false, false),
    wayside_controller::WaysideBlock(16, 0, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 15, 0, false, false, false)};

wayside_controller::Error GetInput(const wayside_controller::InputId input, wayside_controller::IoSignal &signal)
{
    wayside_controller::Error error = wayside_controller::ERROR_INVALID_INPUT;

    if (input < kInputs.size())
    {
        signal = wayside_controller::IOSIGNAL_LOW;

        if (kInputs[input])
        {
            signal = wayside_controller::IOSIGNAL_HIGH;
        }
    }

    return error;
}

wayside_controller::Error SetOutput(const wayside_controller::OutputId output, const wayside_controller::IoSignal signal)
{
    wayside_controller::Error error = wayside_controller::ERROR_INVALID_OUTPUT;

    if (output < WAYSIDE_CONTROLLER_TOTAL_OUTPUTS)
    {
        // TODO NNF-105 set outputs
        LOGGER_UNUSED(signal); // temporary fix to remove compiler warnings
        error = wayside_controller::ERROR_NONE;
    }

    return error;
}

TEST(WaysideControllerTests, Configure)
{
    std::vector<wayside_controller::WaysideBlock> blocks = kBlueLineWaysideBlocks;
    wayside_controller::WaysideController         software_wayside_controller(GetInput);

    // Success
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.Configure(blocks));

    // Empty configuration
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.Configure(std::vector<wayside_controller::WaysideBlock>()));

    // Duplicate block ID
    blocks.push_back(wayside_controller::WaysideBlock(1, 2, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 16, 0, false, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_BLOCK, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Duplicate block ID
    blocks.push_back(wayside_controller::WaysideBlock(1, 3, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 16, 0, false, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_BLOCK, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid track circuit input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 78, 0, false, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid track circuit input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, -1, 0, false, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Duplicate track circuit input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 0, 0, false, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid switch input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 16, 77, true, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid switch input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 16, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT, true, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Invalid switch input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 16, -1, true, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();

    // Duplicate switch input
    blocks.push_back(wayside_controller::WaysideBlock(17, 0, 0, types::BLOCKDIRECTION_UNIDIRECTIONAL, 16, 78, true, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_INPUT, software_wayside_controller.Configure(blocks));
    blocks.pop_back();
}

TEST(WaysideControllerTests, GetCommandedSpeedAndAuthority)
{
    // TODO NNF-144 test commanded speed

    std::array<wayside_controller::IoSignal, 16>                                                                            inputs     = {wayside_controller::IOSIGNAL_LOW};
    std::function<wayside_controller::Error(const wayside_controller::InputId input, wayside_controller::IoSignal &signal)> get_inputs =
        [&inputs](const wayside_controller::InputId input, wayside_controller::IoSignal &signal)
        {
            wayside_controller::Error error = wayside_controller::ERROR_INVALID_INPUT;

            if (input < inputs.size())
            {
                signal = inputs[input];

                error = wayside_controller::ERROR_NONE;
            }

            return error;
        };

    types::TrackCircuitData               track_circuit_data;
    wayside_controller::WaysideController software_wayside_controller(get_inputs, kBlueLineWaysideBlocks);

    // Valid authority
    track_circuit_data.block     = 1;
    track_circuit_data.speed     = 0;
    track_circuit_data.authority = 0;
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(0, track_circuit_data.authority);

    // Valid authority, does not exceed given authority
    track_circuit_data.authority = 9;
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(9, track_circuit_data.authority);

    // Valid authority, does not exceed given authority
    track_circuit_data.authority = 5;
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(5, track_circuit_data.authority);

    // Valid authority to end of track
    track_circuit_data.authority = 10;
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(10, track_circuit_data.authority);

    // Sets safe authority from invalid authority
    track_circuit_data.authority = 16;
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(10, track_circuit_data.authority);

    // Sets safe authority based on occupancies
    track_circuit_data.authority = 10;
    inputs[4]                    = wayside_controller::IOSIGNAL_HIGH;
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(3, track_circuit_data.authority);
    inputs[4] = wayside_controller::IOSIGNAL_LOW;

    // Invalid block
    track_circuit_data.block = -1;
    ASSERT_EQ(wayside_controller::ERROR_INVALID_BLOCK, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(0, track_circuit_data.authority);

    // Invalid block
    track_circuit_data.block = 0;
    ASSERT_EQ(wayside_controller::ERROR_INVALID_BLOCK, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(0, track_circuit_data.authority);

    // Invalid block
    track_circuit_data.block = 17;
    ASSERT_EQ(wayside_controller::ERROR_INVALID_BLOCK, software_wayside_controller.GetCommandedSpeedAndAuthority(track_circuit_data));
    ASSERT_EQ(0, track_circuit_data.authority);

    // Force get input error
    std::function<wayside_controller::Error(const wayside_controller::InputId input, wayside_controller::IoSignal &signal)> get_inputs_none =
        [](const wayside_controller::InputId input, wayside_controller::IoSignal &signal)
        {
            LOGGER_UNUSED(input);
            LOGGER_UNUSED(signal);
            return wayside_controller::ERROR_INVALID_INPUT;
        };
    wayside_controller::WaysideController software_wayside_controlle_no_inputs(get_inputs_none, kBlueLineWaysideBlocks);
    track_circuit_data.block     = 1;
    track_circuit_data.authority = 5;
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controlle_no_inputs.GetCommandedSpeedAndAuthority(track_circuit_data));
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
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetSwitch(5, true));
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetSwitch(5, false));

    // Invalid block
    ASSERT_EQ(wayside_controller::ERROR_INVALID_BLOCK, software_wayside_controller.SetSwitch(17, true));

    // Block does not have switch
    ASSERT_EQ(wayside_controller::ERROR_INVALID_BLOCK, software_wayside_controller.SetSwitch(1, true));
}

TEST(WaysideControllerTests, GetBlockStates)
{
    // TODO NNF-168
}