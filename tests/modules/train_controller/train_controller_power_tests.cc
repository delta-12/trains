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

TEST(TrainControllerPowerTests, CommandedSpeedInputHigherWhenStationary)
{
    train_controller::SoftwareTrainController TC;

    // assert automatic mode
    ASSERT_EQ(0, TC.GetOperationMode()); 

    // train stopped
    TC.SetCurrentSpeed(0);

    // commanded speed passed, faster than current speed
    TC.SetCommandedSpeed(18); 

    // call power calculation
    TC.CalculateCommandedPower();

    // std::cout << TC.GetCommandedPower();
    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}



TEST(TrainControllerPowerTests, CommandedSpeedInputHigherWhenMoving)
{
    train_controller::SoftwareTrainController TC;

    // assert automatic mode
    ASSERT_EQ(0, TC.GetOperationMode()); 

    TC.SetCurrentSpeed(10);

    // commanded speed passed, faster than current speed
    TC.SetCommandedSpeed(15); 

    // call power calculation
    TC.CalculateCommandedPower();

    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}

TEST(TrainControllerPowerTests, CommandedSpeedInputLowerWhenMoving)
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

TEST(TrainControllerPowerTests, CurrentSpeedEqualsSetpointSpeed)
{
    train_controller::SoftwareTrainController TC;

    // assert automatic mode
    ASSERT_EQ(0, TC.GetOperationMode());

    // set current speed and commanded speed to same value
    TC.SetCurrentSpeed(10);
    TC.SetCommandedSpeed(10);

    // call power calculation
    TC.CalculateCommandedPower();

    // assert power is 0 
    ASSERT_EQ(TC.GetCommandedPower(), 0);

    // assert service brake is 0
    ASSERT_EQ(TC.GetServiceBrake(), 0);
}

TEST(TrainControllerPowerTests, DriverSpeedInputHigherWhenStationary) 
{
    train_controller::SoftwareTrainController TC;

    TC.SetOperationMode(1);

    // assert manual mode
    ASSERT_EQ(1, TC.GetOperationMode());

    // train stopped
    TC.SetCurrentSpeed(0);

    // commanded speed passed, faster than current speed
    TC.SetDriverSpeed(18); 

    // call power calculation
    TC.CalculateCommandedPower();

    // std::cout << TC.GetCommandedPower();
    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}

TEST(TrainControllerPowerTests, DriverSpeedInputHigherWhenMoving) 
{
    train_controller::SoftwareTrainController TC;

    TC.SetOperationMode(1);

    // assert manual mode
    ASSERT_EQ(1, TC.GetOperationMode());

    // train moving
    TC.SetCurrentSpeed(5);

    // commanded speed passed, faster than current speed
    TC.SetDriverSpeed(15); 

    // call power calculation
    TC.CalculateCommandedPower();

    // std::cout << TC.GetCommandedPower();
    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}