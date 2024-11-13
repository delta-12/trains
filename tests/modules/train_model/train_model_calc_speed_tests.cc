/*****************************************************************************
* @file train_model_calc_speed_tests.cc
*
* @brief Unit testing for Tick SOurce implementation in train model.
*****************************************************************************/

#include "train_model.h"
#include <unistd.h>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <gtest/gtest.h>



TEST(TrainModelSpeedCalcTests, SpeedCalc1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    types::Second elapsed_time0(0);
    types::Second elapsed_time1(1);

    EXPECT_DOUBLE_EQ(0.0, TM.GetActualSpeed());

    TM.SpeedCalc(elapsed_time0);
    //checking velocity is zero
    EXPECT_DOUBLE_EQ(0.0, TM.GetActualSpeed());

    ////Setting power to 60000 W and checking
    TM.SetCommandedPower(60000);
    EXPECT_DOUBLE_EQ(60000, TM.GetActualPower());

    TM.SpeedCalc(elapsed_time1);

    ASSERT_EQ(.5, TM.GetActualSpeed());

    TM.SpeedCalc(elapsed_time1);

    ASSERT_EQ(1, TM.GetActualSpeed());

}
TEST(TrainModelSpeedCalcTests, SpeedCalc2)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    (*CLOCK).Start();
    usleep(1000000);
    TM.Update();

    //Checking train isnt moving
    ASSERT_EQ(0, TM.GetActualSpeed());

    //Setting commanded power to 60kW
    TM.SetCommandedPower(120000);

    //waiting 1 second
    usleep(1000000);
    TM.Update();

    ASSERT_EQ(.5, round(TM.GetActualSpeed() * 10) / 10);

    usleep(1000000);
    TM.Update();

    ASSERT_EQ(1.0, round(TM.GetActualSpeed() * 10) / 10);

    tick_source.Stop();
}
TEST(TrainModelSpeedCalcTests, SpeedCalc3)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    (*CLOCK).Start();
    usleep(1000000);
    TM.Update();

    //Checking train isnt moving
    ASSERT_EQ(0, TM.GetActualSpeed());

    //Setting commanded power to 60kW
    TM.SetCommandedPower(1000);

    //waiting 1 second
    usleep(100000);
    TM.Update();
    usleep(100000);
    TM.Update();
    usleep(100000);
    TM.Update();
    usleep(100000);
    TM.Update();
    usleep(100000);
    TM.Update();
    usleep(100000);
    TM.Update();
    usleep(100000);
    TM.Update();
    usleep(100000);
    TM.Update();
    usleep(100000);
    TM.Update();
    usleep(100000);
    TM.Update();

    ASSERT_EQ(.2, round(TM.GetActualSpeed() * 10) / 10);

    usleep(1000000);
    TM.Update();

    ASSERT_EQ(.4, round(TM.GetActualSpeed() * 10) / 10);

    tick_source.Stop();
}
TEST(TrainModelSpeedCalcTests, SpeedCalc4)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    (*CLOCK).Start();
    usleep(1000000);
    TM.Update();

    //Checking train isnt moving
    ASSERT_EQ(0, TM.GetActualSpeed());

    //Setting commanded power to 60kW
    TM.SetCommandedPower(60000);

    //waiting 5 seconds
    usleep(5000000);
    TM.Update();

    ASSERT_EQ(2.5, round(TM.GetActualSpeed() * 10) / 10);

    TM.SetBrake(.5);

    usleep(1000000);
    TM.Update();

    ASSERT_EQ(1.9, round(TM.GetActualSpeed() * 10) / 10);

    tick_source.Stop();
}
TEST(TrainModelSpeedCalcTests, SpeedCalc5)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    (*CLOCK).Start();
    usleep(1000000);
    TM.Update();

    //Checking train isnt moving
    ASSERT_EQ(0, TM.GetActualSpeed());

    //Setting commanded power to 60kW
    TM.SetCommandedPower(60000);

    //waiting 5 secondd
    usleep(5000000);
    TM.Update();

    ASSERT_EQ(2.5, round(TM.GetActualSpeed() * 10) / 10);

    TM.SetBrake(.25);

    usleep(1000000);
    TM.Update();

    ASSERT_EQ(2.2, round(TM.GetActualSpeed() * 10) / 10);

    tick_source.Stop();
}
TEST(TrainModelSpeedCalcTests, SpeedCalc6)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    (*CLOCK).Start();
    usleep(1000000);
    TM.Update();

    //Checking train isnt moving
    ASSERT_EQ(0, TM.GetActualSpeed());

    //Setting commanded power to 60kW
    TM.SetCommandedPower(60000);

    //waiting 5 seconds
    usleep(10000000);
    TM.Update();

    ASSERT_EQ(5, round(TM.GetActualSpeed() * 10) / 10);

    TM.SetEmergencyBrake(true);

    usleep(1000000);
    TM.Update();

    ASSERT_EQ(2.27, round(TM.GetActualSpeed() * 100) / 100);

    TM.SetEmergencyBrake(false);

    tick_source.Stop();
}