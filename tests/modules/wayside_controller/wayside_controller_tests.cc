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

static const std::vector<wayside_controller::BlockIo> kBlueLineBlockIo = {
    wayside_controller::BlockIo(1, 0, 0, false, false),
    wayside_controller::BlockIo(2, 1, 0, false, false),
    wayside_controller::BlockIo(3, 2, 0, false, false),
    wayside_controller::BlockIo(4, 3, 0, false, false),
    wayside_controller::BlockIo(5, 4, 78, true, false),
    wayside_controller::BlockIo(6, 5, 0, false, false),
    wayside_controller::BlockIo(7, 6, 0, false, false),
    wayside_controller::BlockIo(8, 7, 0, false, false),
    wayside_controller::BlockIo(9, 8, 0, false, false),
    wayside_controller::BlockIo(10, 9, 0, false, false),
    wayside_controller::BlockIo(11, 10, 0, false, false),
    wayside_controller::BlockIo(12, 11, 0, false, false),
    wayside_controller::BlockIo(13, 12, 0, false, false),
    wayside_controller::BlockIo(14, 13, 0, false, false),
    wayside_controller::BlockIo(15, 14, 0, false, false),
    wayside_controller::BlockIo(16, 15, 0, false, false)};

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

TEST(WaysideControllerTests, SetBlockMap)
{
    std::vector<wayside_controller::BlockIo> block_io_map = kBlueLineBlockIo;
    wayside_controller::WaysideController    software_wayside_controller(GetInput);

    // Success
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetBlockMap(block_io_map));

    // Empty map
    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetBlockMap(std::vector<wayside_controller::BlockIo>()));

    // Duplicate block ID
    block_io_map.push_back(wayside_controller::BlockIo(1, 16, 0, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_BLOCK, software_wayside_controller.SetBlockMap(block_io_map));
    block_io_map.pop_back();

    // Invalid track circuit input
    block_io_map.push_back(wayside_controller::BlockIo(17, 78, 0, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_io_map));
    block_io_map.pop_back();

    // Invalid track circuit input
    block_io_map.push_back(wayside_controller::BlockIo(17, -1, 0, false, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_io_map));
    block_io_map.pop_back();

    // Duplicate track circuit input
    block_io_map.push_back(wayside_controller::BlockIo(17, 0, 0, false, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_INPUT, software_wayside_controller.SetBlockMap(block_io_map));
    block_io_map.pop_back();

    // Invalid switch input
    block_io_map.push_back(wayside_controller::BlockIo(17, 16, 77, true, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_io_map));
    block_io_map.pop_back();

    // Invalid switch input
    block_io_map.push_back(wayside_controller::BlockIo(17, 16, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT, true, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_io_map));
    block_io_map.pop_back();

    // Invalid switch input
    block_io_map.push_back(wayside_controller::BlockIo(17, 16, -1, true, false));
    ASSERT_EQ(wayside_controller::ERROR_INVALID_INPUT, software_wayside_controller.SetBlockMap(block_io_map));
    block_io_map.pop_back();

    // Duplicate switch input
    block_io_map.push_back(wayside_controller::BlockIo(17, 16, 78, true, false));
    ASSERT_EQ(wayside_controller::ERROR_DUPLICATE_INPUT, software_wayside_controller.SetBlockMap(block_io_map));
    block_io_map.pop_back();
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

    ASSERT_EQ(wayside_controller::ERROR_NONE, software_wayside_controller.SetBlockMap(kBlueLineBlockIo));

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