/*****************************************************************************
* @file train_controller_operation_mode_tests.cc
*
* @brief Unit testing for changing the operstion mode of the train controller.
*****************************************************************************/
#include "train_controller.h"
#include <unistd.h>
#include <iomanip>
#include <gtest/gtest.h>

TEST(TrainControllerOperationModeTests, SettingOperationMode)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    //assert operation mode starts in automatic

    ASSERT_EQ(0, TC.GetOperationMode());

    // switch to manual mode and assert it worked
    TC.SetOperationMode(1);
    ASSERT_EQ(1, TC.GetOperationMode());

    // attempt to switch back to automatic mode and assert still in manual
    TC.SetOperationMode(0);
    ASSERT_EQ(1, TC.GetOperationMode());
}