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

static const std::vector<wayside_controller::BlockInputs> kBlueLineBlockInputs = {
    wayside_controller::BlockInputs(1, 0, 0, false, false, false),
    wayside_controller::BlockInputs(2, 1, 0, false, false, false),
    wayside_controller::BlockInputs(3, 2, 0, false, false, false),
    wayside_controller::BlockInputs(4, 3, 0, false, false, false),
    wayside_controller::BlockInputs(5, 4, 78, true, false, false),
    wayside_controller::BlockInputs(6, 5, 0, false, false, false),
    wayside_controller::BlockInputs(7, 6, 0, false, false, false),
    wayside_controller::BlockInputs(8, 7, 0, false, false, false),
    wayside_controller::BlockInputs(9, 8, 0, false, false, false),
    wayside_controller::BlockInputs(10, 9, 0, false, false, false),
    wayside_controller::BlockInputs(11, 10, 0, false, false, false),
    wayside_controller::BlockInputs(12, 11, 0, false, false, false),
    wayside_controller::BlockInputs(13, 12, 0, false, false, false),
    wayside_controller::BlockInputs(14, 13, 0, false, false, false),
    wayside_controller::BlockInputs(15, 14, 0, false, false, false),
    wayside_controller::BlockInputs(16, 15, 0, false, false, false)};

static const std::vector<wayside_controller::BlockConnection> kBlueLineBlockConnections = {}; // TODO test build graph

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
    std::vector<wayside_controller::BlockInputs>     block_input_map   = kBlueLineBlockInputs;
    std::vector<wayside_controller::BlockConnection> block_connections = kBlueLineBlockConnections;
    wayside_controller::WaysideController            software_wayside_controller(GetInput);

    // Success
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.Configure({block_input_map, block_connections}));

    // Empty configuration
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.Configure({std::vector<wayside_controller::BlockInputs>(), std::vector<wayside_controller::BlockConnection>()}));

    // Duplicate block ID
    block_input_map.push_back(wayside_controller::BlockInputs(1, 16, 0, false, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_BLOCK, software_wayside_controller.Configure({block_input_map, block_connections}));
    block_input_map.pop_back();

    // Invalid track circuit input
    block_input_map.push_back(wayside_controller::BlockInputs(17, 78, 0, false, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure({block_input_map, block_connections}));
    block_input_map.pop_back();

    // Invalid track circuit input
    block_input_map.push_back(wayside_controller::BlockInputs(17, -1, 0, false, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure({block_input_map, block_connections}));
    block_input_map.pop_back();

    // Duplicate track circuit input
    block_input_map.push_back(wayside_controller::BlockInputs(17, 0, 0, false, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_INPUT, software_wayside_controller.Configure({block_input_map, block_connections}));
    block_input_map.pop_back();

    // Invalid switch input
    block_input_map.push_back(wayside_controller::BlockInputs(17, 16, 77, true, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure({block_input_map, block_connections}));
    block_input_map.pop_back();

    // Invalid switch input
    block_input_map.push_back(wayside_controller::BlockInputs(17, 16, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT, true, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure({block_input_map, block_connections}));
    block_input_map.pop_back();

    // Invalid switch input
    block_input_map.push_back(wayside_controller::BlockInputs(17, 16, -1, true, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.Configure({block_input_map, block_connections}));
    block_input_map.pop_back();

    // Duplicate switch input
    block_input_map.push_back(wayside_controller::BlockInputs(17, 16, 78, true, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_INPUT, software_wayside_controller.Configure({block_input_map, block_connections}));
    block_input_map.pop_back();

    // TODO test build graph
}

TEST(WaysideControllerTests, GetCommandedSpeedAndAuthority)
{
    // TODO NNF-144
}

TEST(WaysideControllerTests, SetMaintenanceMode)
{
    // TODO NNF-105
}

TEST(WaysideControllerTests, SetSwitch)
{
    // TODO NNF-105

    wayside_controller::WaysideController software_wayside_controller(GetInput);
    wayside_controller::Configuration     configuration(kBlueLineBlockInputs, kBlueLineBlockConnections);

    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.Configure(configuration));

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