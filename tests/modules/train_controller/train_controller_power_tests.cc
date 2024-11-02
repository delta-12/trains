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
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    // assert automatic mode
    ASSERT_EQ(false, TC.GetOperationMode());

    // train stopped
    TC.SetCurrentSpeed(0);

    // commanded speed passed, faster than current speed
    TC.SetCommandedSpeed(18);

    // call power calculation
    TC.CalculateCommandedPower(elapsed_time1);

    // std::cout << TC.GetCommandedPower();
    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}



TEST(TrainControllerPowerTests, CommandedSpeedInputHigherWhenMoving)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    // assert automatic mode
    ASSERT_EQ(false, TC.GetOperationMode());

    TC.SetCurrentSpeed(10);

    // commanded speed passed, faster than current speed
    TC.SetCommandedSpeed(15);

    usleep(3000000);

    // call power calculation
    TC.Update();

    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}

TEST(TrainControllerPowerTests, CommandedSpeedInputLowerWhenMoving)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    // assert automatic mode
    ASSERT_EQ(false, TC.GetOperationMode());

    // train going faster than commanded speed that will be input
    TC.SetCurrentSpeed(18);

    // commanded speed passed, slower than current speed
    TC.SetCommandedSpeed(15);

    // call power calculation
    TC.CalculateCommandedPower(elapsed_time1);

    // assert power is equal to 0
    ASSERT_EQ(TC.GetCommandedPower(), 0);

    // assert service brake is on
    ASSERT_GT(TC.GetServiceBrake(), 0);
}

TEST(TrainControllerPowerTests, CurrentSpeedEqualsSetpointSpeed)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    // assert automatic mode
    ASSERT_EQ(false, TC.GetOperationMode());

    // set current speed and commanded speed to same value
    TC.SetCurrentSpeed(10);
    TC.SetCommandedSpeed(10);

    // call power calculation
    TC.CalculateCommandedPower(elapsed_time1);

    // assert power is 0
    ASSERT_EQ(TC.GetCommandedPower(), 0);

    // assert service brake is 0
    ASSERT_EQ(TC.GetServiceBrake(), 0);
}

TEST(TrainControllerPowerTests, DriverSpeedInputHigherWhenStationary)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    TC.SetOperationMode(true);

    // assert manual mode
    ASSERT_EQ(true, TC.GetOperationMode());

    // train stopped
    TC.SetCurrentSpeed(0);

    // commanded speed passed, faster than current speed
    TC.SetDriverSpeed(18);

    // call power calculation
    TC.CalculateCommandedPower(elapsed_time1);

    // std::cout << TC.GetCommandedPower();
    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}

TEST(TrainControllerPowerTests, DriverSpeedInputHigherWhenMoving)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    TC.SetOperationMode(true);

    // assert manual mode
    ASSERT_EQ(true, TC.GetOperationMode());

    // train moving
    TC.SetCurrentSpeed(5);

    // commanded speed passed, faster than current speed
    TC.SetDriverSpeed(15);

    // call power calculation
    TC.CalculateCommandedPower(elapsed_time1);

    // assert power is greater than 0
    ASSERT_GT(TC.GetCommandedPower(), 0);
}

TEST(TrainControllerPowerTests, NegativeDriverSpeed)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    // go to manual mode
    TC.SetOperationMode(true);

    // assert manual mode
    ASSERT_EQ(true, TC.GetOperationMode());

    // random current speed
    TC.SetCurrentSpeed(18);

    // negative driver speed
    TC.SetDriverSpeed(-5);

    // calculate power
    TC.CalculateCommandedPower(elapsed_time1);

    // assert driver speed and power = 0 and service brake is on
    ASSERT_EQ(TC.GetDriverSpeed(), 0);
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);
}

TEST(TrainControllerPowerTests, DriverSpeedOverSpeedLimit)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    // go to manual mode
    TC.SetOperationMode(true);

    // assert manual mode
    ASSERT_EQ(true, TC.GetOperationMode());

    // random current speed
    TC.SetCurrentSpeed(0);

    // driver speed above speed limit
    TC.SetDriverSpeed(700);

    // calculate power
    TC.CalculateCommandedPower(elapsed_time1);

    // assert driver speed and power = 0 and service brake is on
    ASSERT_NEAR(TC.GetDriverSpeed(), 31.0599, 0.05);
    // ^ 31 b/c set driver capped at 13.8889 m/s (assuming 50 km/hr speed limit)
    // 13.889 * 2.23694 (conv for m/s to mph) ~= 31

    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);
}

TEST(TrainControllerPowerTests, EngineFailure)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    // make train have some power output and speed
    TC.SetCurrentSpeed(10);
    TC.SetCommandedSpeed(15);
    TC.CalculateCommandedPower(elapsed_time1);

    // turn on engine failure
    TC.SetEngineFailure(true);

    // call failure state check
    TC.CalculateCommandedPower(elapsed_time1);

    // assert power = 0
    ASSERT_EQ(TC.GetCommandedPower(), 0);

    // assert emergency brake is on
    ASSERT_EQ(TC.GetEmergencyBrake(), true);

    // turn off emeergency brake and brake failure
    TC.SetEmergencyBrake(false);
    TC.SetEngineFailure(false);

    // make train have service brake
    TC.SetCurrentSpeed(18);
    TC.SetCommandedSpeed(15);
    TC.CalculateCommandedPower(elapsed_time1);

    // turn brake failure back on
    TC.SetEngineFailure(true);

    // make sure service brake is on
    ASSERT_GT(TC.GetServiceBrake(), 0);

    // call failure state check
    TC.CalculateCommandedPower(elapsed_time1);

    // assert service brake is off, emergency brake is on and power is 0
    ASSERT_EQ(TC.GetServiceBrake(), 0);
    ASSERT_EQ(TC.GetEmergencyBrake(), true);
    ASSERT_EQ(TC.GetCommandedPower(), 0);
}

TEST(TrainControllerPowerTests, BrakeFailure)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    // make train have some power output and speed
    TC.SetCurrentSpeed(10);
    TC.SetCommandedSpeed(15);
    TC.CalculateCommandedPower(elapsed_time1);

    // turn on engine failure
    TC.SetBrakeFailure(true);

    // call failure state check
    TC.CalculateCommandedPower(elapsed_time1);

    // assert power = 0
    ASSERT_EQ(TC.GetCommandedPower(), 0);

    // assert emergency brake is on
    ASSERT_EQ(TC.GetEmergencyBrake(), true);

    // turn off emeergency brake and brake failure
    TC.SetEmergencyBrake(false);
    TC.SetBrakeFailure(false);

    // make train have service brake
    TC.SetCurrentSpeed(18);
    TC.SetCommandedSpeed(15);
    TC.CalculateCommandedPower(elapsed_time1);

    // turn brake faialure back on
    TC.SetBrakeFailure(true);
    std::cout << TC.GetServiceBrake();

    // make sure service brake is on
    ASSERT_GT(TC.GetServiceBrake(), 0);

    // call failure state check
    TC.CalculateCommandedPower(elapsed_time1);

    // assert service brake is off, emergency brake is on and power is 0
    ASSERT_EQ(TC.GetServiceBrake(), 0);
    ASSERT_EQ(TC.GetEmergencyBrake(), true);
    ASSERT_EQ(TC.GetCommandedPower(), 0);
}

TEST(TrainControllerPowerTests, SignalPickupFailure)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    types::Second elapsed_time1(1);

    // make train have some power output and speed
    TC.SetCurrentSpeed(10);
    TC.SetCommandedSpeed(15);
    TC.CalculateCommandedPower(elapsed_time1);

    // turn on engine failure
    TC.SetSignalPickupFailure(true);

    // call failure state check
    TC.CalculateCommandedPower(elapsed_time1);

    // assert power = 0
    ASSERT_EQ(TC.GetCommandedPower(), 0);

    // assert emergency brake is on
    ASSERT_EQ(TC.GetEmergencyBrake(), true);

    // turn off emergency brake and brake failure
    TC.SetEmergencyBrake(false);
    TC.SetSignalPickupFailure(false);

    // make train have service brake
    TC.SetCurrentSpeed(18);
    TC.SetCommandedSpeed(15);
    TC.CalculateCommandedPower(elapsed_time1);

    // turn brake failure back on
    TC.SetSignalPickupFailure(true);

    // make sure service brake is on
    ASSERT_GT(TC.GetServiceBrake(), 0);

    // call failure state check
    TC.CalculateCommandedPower(elapsed_time1);

    // assert service brake is off, emergency brake is on and power is 0
    ASSERT_EQ(TC.GetServiceBrake(), 0);
    ASSERT_EQ(TC.GetEmergencyBrake(), true);
    ASSERT_EQ(TC.GetCommandedPower(), 0);
}