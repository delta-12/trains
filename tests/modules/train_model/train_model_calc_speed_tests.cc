/*****************************************************************************
* @file train_model_calc_speed_tests.cc
*
* @brief Unit testing for Tick SOurce implementation in train controller.
*****************************************************************************/

#include "train_model.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <gtest/gtest.h>



TEST(TrainModelSpeedCalcTests, SpeedCalc1)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TC(CLOCK);


    types::Second elapsed_time0(0);
    types::Second elapsed_time1(1);
    types::Second elapsed_time2(2);
    types::Second elapsed_time3(3);



    //usleep(1000000);
    //TC.UpdateDistanceTravelled(elapsed_time0);
//
    ////Checking no distance has been travelled
    //EXPECT_DOUBLE_EQ(0, TC.GetDistanceTravelled());
//
//
    ////Setting current speed to 10 m/s
    //TC.SetCurrentSpeed(10);
//
    //TC.UpdateDistanceTravelled(elapsed_time2);
//
    ////Checking if the distance travelled corresponds to the time passed and the current speed
    //EXPECT_DOUBLE_EQ(20, TC.GetDistanceTravelled());
//
//
//
    ////Setting current speed to 5 m/s
    //TC.SetCurrentSpeed(5);
//
    //TC.UpdateDistanceTravelled(elapsed_time1);
//
    ////Checking if the distance travelled corresponds to the time passed and the current speed
    //EXPECT_DOUBLE_EQ(25, TC.GetDistanceTravelled());
//
//
//
    ////Setting current speed to 2.5 m/s
    //TC.SetCurrentSpeed(2.5);
//
    ////Waiting for 3 seconds
    //TC.UpdateDistanceTravelled(elapsed_time3);
//
    ////Checking if the distance travelled corresponds to the time passed and the current speed
    //EXPECT_DOUBLE_EQ(32.5, TC.GetDistanceTravelled());
}