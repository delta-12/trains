/*****************************************************************************
* @file train_controller_operation_mode_tests.cc
*
* @brief Unit testing for changing the operstion mode of the train controller.
*****************************************************************************/
#include "train_controller.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <gtest/gtest.h>

TEST(TrainControllerPowerTests, CommandedSpeedInputHigher)
{
    train_controller::SoftwareTrainController TC;

    // assert automatic mode
    ASSERT_EQ(0, TC.GetOperationMode()); 

    // train going slower than commanded speed that will be input
    TC.SetCurrentSpeed(30);

    // commanded speed passed, faster than current speed
    TC.SetCommandedSpeed(50); 

    // call power calculation
    TC.CalculateCommandedPower();

    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}

/*TEST(TrainControllerPowerTests, CommandedSpeedInputLower)
{
    train_controller::SoftwareTrainController TC;

    // assert automatic mode
    ASSERT_EQ(0, TC.GetOperationMode()); 

    // train going faster than commanded speed that will be input
    TC.SetCurrentSpeed(50);

    // commanded speed passed, slower than current speed
    TC.SetCommandedSpeed(30); 

    // call power calculation
    TC.CalculateCommandedPower();

    // assert power is equal to 0
    ASSERT_EQ(TC.GetCommandedPower(), 0);

    // assert service brake is on
    ASSERT_GT(TC.GetServiceBrake(), 0);
}*/