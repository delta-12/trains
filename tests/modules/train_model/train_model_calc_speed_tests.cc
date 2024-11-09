/*****************************************************************************
* @file train_model_calc_speed_tests.cc
*
* @brief Unit testing for Tick SOurce implementation in train model.
*****************************************************************************/

#include "train_model.h"
#include <unistd.h>
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
    types::Second elapsed_time2(2);
    types::Second elapsed_time3(3);

    EXPECT_DOUBLE_EQ(0.0, TM.GetActualSpeed());

    TM.SpeedCalc(elapsed_time0);
    //checking velocity is zero
    EXPECT_DOUBLE_EQ(0.0, TM.GetActualSpeed());

    ////Setting power to 60000 W and checking
    TM.SetCommandedPower(60000);
    EXPECT_DOUBLE_EQ(60000, TM.GetActualPower());

    TM.SpeedCalc(elapsed_time1);

    bool inBetween = (TM.GetActualSpeed() >= 3.23 && TM.GetActualSpeed() <= 3.25);
    ////Checking if the velocity corresponds to the time passed and the current speed
    EXPECT_DOUBLE_EQ(inBetween, true);
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

    //Setting commanded power to 40kW
    TM.SetCommandedPower(40000);

    //waiting 1 second
    usleep(1000000);
    TM.Update();

    bool range1 = (TM.GetActualSpeed() > 6.3 && TM.GetActualSpeed() < 6.5);
    //6.4846508368595535

    //ASSERT_EQ(6, TM.GetActualSpeed()); test just to see exact value

    ASSERT_EQ(range1, true);

    usleep(1000000);
    TM.Update();

    bool range2 = (TM.GetActualSpeed() > 6.9 && TM.GetActualSpeed() < 7.1);

    ASSERT_EQ(range2, true);

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

    //Setting commanded power to 40kW
    TM.SetCommandedPower(40000);

    //waiting 1 second
    usleep(1000000);
    TM.Update();

    bool range1 = (TM.GetActualSpeed() > 6.3 && TM.GetActualSpeed() < 6.5);
    ASSERT_EQ(range1, true);

    TM.SetBrake(.5);

    usleep(1000000);
    TM.Update();

    bool range2 = (TM.GetActualSpeed() > 4.5 && TM.GetActualSpeed() < 4.9);

    ASSERT_EQ(range2, true);
    //ASSERT_EQ(6, TM.GetActualSpeed()); //test just to see exact value
    //4.6851193164973193

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

    //Setting commanded power to 40kW
    TM.SetCommandedPower(40000);

    //waiting 1 second
    usleep(1000000);
    TM.Update();

    bool range1 = (TM.GetActualSpeed() > 6.3 && TM.GetActualSpeed() < 6.5);
    ASSERT_EQ(range1, true);

    TM.SetBrake(.25);

    usleep(1000000);
    TM.Update();

    bool range2 = (TM.GetActualSpeed() > 5.4 && TM.GetActualSpeed() < 5.7);

    ASSERT_EQ(range2, true);
    //ASSERT_EQ(5, TM.GetActualSpeed()); //test just to see exact value
    //5.5734481174029042

    tick_source.Stop();
}