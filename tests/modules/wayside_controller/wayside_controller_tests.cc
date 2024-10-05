/*****************************************************************************
 * @file wayside_controller_tests.cc
 *
 * @brief Unit testing for WaysideController.
 *****************************************************************************/

#include <gtest/gtest.h>

#include "wayside_controller.h"

static const std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> kInputs = {true, true, false, false, true, false, false, false, false, false, false, false, true, false, true, false, false, true, false, false, true, true, true, true, false, false, false, true, false, true, false, true, false, true, false, false, true, false, false, false, false,
                                                                               false, false, false, true, true, false, false, true, true, false, false, false, false, true, true, false, true, true, false, true, false, false, true, true, true, false, false, false, false, false, true, false, true, false, true, false, false, false, true, false, true};

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
    // TODO
}

TEST(WaysideControllerTests, SetOutput)
{
    // TODO NNF-105
}

TEST(WaysideControllerTests, GetInput)
{
    bool input_state;
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
    // TODO
}

TEST(WaysideControllerTests, SetSwitch)
{
    // TODO NNF-105

    // TODO test covering existing code
}

TEST(WaysideControllerTests, GetBlockStates)
{
    // TODO NNF-168
}