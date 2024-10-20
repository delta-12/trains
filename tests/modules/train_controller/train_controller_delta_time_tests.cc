/*****************************************************************************
* @file train_controller_delta_time_tests.cc
*
* @brief Unit testing for Tick SOurce implementation in train controller.
*****************************************************************************/

#include <unistd.h>

#include <iostream>
#include <iomanip>


#include <gtest/gtest.h>

#include "train_controller.h"



TEST(TrainControllerDeltaTimeTests, GetDeltaTime)
{
    TickSource tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource> CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    
    (*CLOCK).Start();
    usleep(1000000);
    //(*CLOCK).Stop();

    TC.Update();

    //Checking if the delta time corresponds to the 1 second wait time
    EXPECT_DOUBLE_EQ(1, TC.GetDeltaTime());

    //(*CLOCK).Start();
    usleep(2000000);
    //(*CLOCK).Stop();

    TC.Update();

    //Checking if the delta time corresponds to the 2 second wait time
    EXPECT_DOUBLE_EQ(2, TC.GetDeltaTime());

    (*CLOCK).Stop();
}

TEST(TrainControllerDeltaTimeTests, DistanceTravelled1)
{    
    TickSource tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource> CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    
    
    (*CLOCK).Start();
    usleep(1000000);
    TC.UpdateDistanceTravelled(0);

    //Checking no distance has been travelled
    EXPECT_DOUBLE_EQ(0, TC.GetDistanceTravelled());



    //Setting current speed to 10 m/s
    TC.SetCurrentSpeed(10);

    TC.UpdateDistanceTravelled(2);
    
    //Checking if the distance travelled corresponds to the time passed and the current speed
    EXPECT_DOUBLE_EQ(20, TC.GetDistanceTravelled());



    //Setting current speed to 5 m/s
    TC.SetCurrentSpeed(5);

    TC.UpdateDistanceTravelled(1);
    
    //Checking if the distance travelled corresponds to the time passed and the current speed
    EXPECT_DOUBLE_EQ(25, TC.GetDistanceTravelled());



    //Setting current speed to 2.5 m/s
    TC.SetCurrentSpeed(2.5);

    //Waiting for 3 seconds
    TC.UpdateDistanceTravelled(3);
    
    //Checking if the distance travelled corresponds to the time passed and the current speed
    EXPECT_DOUBLE_EQ(32.5, TC.GetDistanceTravelled());

    (*CLOCK).Stop();
}


TEST(TrainControllerDeltaTimeTests, DistanceTravelled2)
{
    TickSource tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource> CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);

    (*CLOCK).Start();
    usleep(1000000);
    TC.Update();

    //Checking no distance has been travelled
    ASSERT_EQ(0, TC.GetDistanceTravelled());

    //Setting current speed to 10 m/s
    TC.SetCurrentSpeed(10);

    //waiting 0.9 seconds
    usleep(900000);
    TC.Update();
    
    //Checking if the distance travelled corresponds to the time passed and the current speed
    EXPECT_DOUBLE_EQ(9, TC.GetDistanceTravelled());

    tick_source.Stop();
}
