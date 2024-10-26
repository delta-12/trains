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

TEST(TrainControllerPowerTests, IncreaseCommandedPowerFromStationary)
{
    train_controller::SoftwareTrainController TC;

    // assert automatic mode
    ASSERT_EQ(0, TC.GetOperationMode()); 

    // commanded speed passed, faster than current speed
    TC.SetCommandedSpeed(18); 

    // call power calculation
    TC.CalculateCommandedPower();

    // std::cout << TC.GetCommandedPower();
    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);

    // train going slower than commanded speed that will be input
    TC.SetCurrentSpeed(15);
}



TEST(TrainControllerPowerTests, IncreaseCommandedPowerInMoving)
{
    train_controller::SoftwareTrainController TC;

    // assert automatic mode
    ASSERT_EQ(0, TC.GetOperationMode()); 


    TC.SetCurrentSpeed(10);

    // commanded speed passed, faster than current speed
    TC.SetCommandedSpeed(18); 

    // call power calculation
    TC.CalculateCommandedPower();

    // std::cout << TC.GetCommandedPower();
    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}

TEST(TrainControllerPowerTests, CommandedSpeedInputLower)
{
    train_controller::SoftwareTrainController TC;

    // assert automatic mode
    ASSERT_EQ(0, TC.GetOperationMode()); 

    // train going faster than commanded speed that will be input
    TC.SetCurrentSpeed(18);

    // commanded speed passed, slower than current speed
    TC.SetCommandedSpeed(15); 

    // call power calculation
    TC.CalculateCommandedPower();

    // assert power is equal to 0
    ASSERT_EQ(TC.GetCommandedPower(), 0);

    // assert service brake is on
    ASSERT_GT(TC.GetServiceBrake(), 0);
}