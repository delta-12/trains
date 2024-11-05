/*****************************************************************************
* @file train_model_calc_speed_tests.cc
*
* @brief Unit testing for Tick SOurce implementation in train model.
*****************************************************************************/

#include "train_model.h"
#include <unistd.h>
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