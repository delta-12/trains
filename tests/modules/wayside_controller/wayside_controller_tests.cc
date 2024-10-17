/*****************************************************************************
* @file wayside_controller_tests.cc
*
* @brief Unit testing for WaysideController.
*****************************************************************************/

#include <gtest/gtest.h>

#include "wayside_controller.h"

static const std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> kInputs = {true, true, false, false, true, false, false, false, false, false, false, false, true, false, true, false, false, true, false, false, true, true, true, true, false, false, false, true, false, true, false, true, false, true, false, false, true, false, false, false, false,
                                                                               false, false, false, true, true, false, false, true, true, false, false, false, false, true, true, false, true, true, false, true, false, false, true, true, true, false, false, false, false, false, true, false, true, false, true, false, false, false, true, false, true};

static const std::vector<wayside_controller::BlockInputs> kBlueLineBlockInputs = {
    wayside_controller::BlockInputs(1, 0, 0, false, false),
    wayside_controller::BlockInputs(2, 1, 0, false, false),
    wayside_controller::BlockInputs(3, 2, 0, false, false),
    wayside_controller::BlockInputs(4, 3, 0, false, false),
    wayside_controller::BlockInputs(5, 4, 75, true, false),
    wayside_controller::BlockInputs(6, 5, 0, false, false),
    wayside_controller::BlockInputs(7, 6, 0, false, false),
    wayside_controller::BlockInputs(8, 7, 0, false, false),
    wayside_controller::BlockInputs(9, 8, 0, false, false),
    wayside_controller::BlockInputs(10, 9, 0, false, false),
    wayside_controller::BlockInputs(11, 10, 0, false, false),
    wayside_controller::BlockInputs(12, 11, 0, false, false),
    wayside_controller::BlockInputs(13, 12, 0, false, false),
    wayside_controller::BlockInputs(14, 13, 0, false, false),
    wayside_controller::BlockInputs(15, 14, 0, false, false),
    wayside_controller::BlockInputs(16, 15, 0, false, false)};

void GetInputs(std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> &inputs)
{
    for (std::size_t i = 0; i < inputs.size(); i++)
    {
        inputs[i] = kInputs[i];
    }
}

wayside_controller::Error SetOutput(const wayside_controller::OutputId output, const bool state)
{
    wayside_controller::Error error = wayside_controller::ERROR_INVALID_OUTPUT;

    if (output < WAYSIDE_CONTROLLER_TOTAL_OUTPUTS)
    {
        // TODO NNF-105 set outputs
        error = wayside_controller::ERROR_NONE;
    }

    return error;
}

TEST(WaysideControllerTests, SetBlockMap)
{
    std::vector<wayside_controller::BlockInputs> block_inputs_map = kBlueLineBlockInputs;
    wayside_controller::WaysideController        software_wayside_controller(GetInputs, SetOutput);

    // Success
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetBlockMap(block_inputs_map));

    // Empty map
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetBlockMap(std::vector<wayside_controller::BlockInputs>()));

    // Duplicate block ID
    block_inputs_map.push_back(wayside_controller::BlockInputs(1, 16, 0, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_BLOCK, software_wayside_controller.SetBlockMap(block_inputs_map));
    block_inputs_map.pop_back();

    // Invalid track circuit input
    block_inputs_map.push_back(wayside_controller::BlockInputs(17, 75, 0, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_inputs_map));
    block_inputs_map.pop_back();

    // Invalid track circuit input
    block_inputs_map.push_back(wayside_controller::BlockInputs(17, -1, 0, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_inputs_map));
    block_inputs_map.pop_back();

    // Duplicate track circuit input
    block_inputs_map.push_back(wayside_controller::BlockInputs(17, 0, 0, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_INPUT, software_wayside_controller.SetBlockMap(block_inputs_map));
    block_inputs_map.pop_back();

    // Invalid switch input
    block_inputs_map.push_back(wayside_controller::BlockInputs(17, 16, 74, true, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_inputs_map));
    block_inputs_map.pop_back();

    // Invalid switch input
    block_inputs_map.push_back(wayside_controller::BlockInputs(17, 16, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT, true, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_inputs_map));
    block_inputs_map.pop_back();

    // Invalid switch input
    block_inputs_map.push_back(wayside_controller::BlockInputs(17, 16, -1, true, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_inputs_map));
    block_inputs_map.pop_back();

    // Duplicate switch input
    block_inputs_map.push_back(wayside_controller::BlockInputs(17, 16, 75, true, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_INPUT, software_wayside_controller.SetBlockMap(block_inputs_map));
    block_inputs_map.pop_back();
}

TEST(WaysideControllerTests, SetOutput)
{
    // TODO NNF-105
}

TEST(WaysideControllerTests, GetInput)
{
    bool                                  input_state;
    wayside_controller::WaysideController software_wayside_controller(GetInputs, SetOutput);

    software_wayside_controller.ScanInputs();

    for (std::size_t i = 0; i < kInputs.size(); i++)
    {
        ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.GetInput((wayside_controller::InputId)i, input_state));
        ASSERT_EQ(kInputs[i], input_state);
    }

    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.GetInput(-1, input_state));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.GetInput(kInputs.size(), input_state));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.GetInput(kInputs.size() + 1, input_state));
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

    bool                                  input_state;
    wayside_controller::WaysideController software_wayside_controller(GetInputs, SetOutput);

    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetBlockMap(kBlueLineBlockInputs));

    // Valid switch
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetSwitch(5, true));
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.GetInput(75, input_state));
    ASSERT_TRUE(input_state);
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetSwitch(5, false));
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.GetInput(75, input_state));
    ASSERT_FALSE(input_state);

    // Invalid block
    ASSERT_EQ(wayside_controller::ERROR_INVALID_BLOCK, software_wayside_controller.SetSwitch(17, true));

    // Block does not have switch
    ASSERT_EQ(wayside_controller::ERROR_INVALID_BLOCK, software_wayside_controller.SetSwitch(1, true));
}

TEST(WaysideControllerTests, GetBlockStates)
{
    // TODO NNF-168
}