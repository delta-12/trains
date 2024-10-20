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
    ASSERT_NEAR(1, TC.GetDeltaTime(),0.02);

    //(*CLOCK).Start();
    usleep(2000000);
    //(*CLOCK).Stop();

    TC.Update();

    //Checking if the delta time corresponds to the 2 second wait time
    ASSERT_NEAR(2, TC.GetDeltaTime(),0.02);

    (*CLOCK).Stop();
}

TEST(TrainControllerDeltaTimeTests, DistanceTravelled1)
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

    //waiting 2 seconds
    usleep(2000000);
    TC.Update();
    
    //Checking if the distance travelled corresponds to the time passed and the current speed
    ASSERT_NEAR(20, TC.GetDistanceTravelled(),0.1);



    //Setting current speed to 5 m/s
    TC.SetCurrentSpeed(5);

    //waiting 1 seconds
    usleep(1000000);
    TC.Update();
    
    //Checking if the distance travelled corresponds to the time passed and the current speed
    ASSERT_NEAR(25, TC.GetDistanceTravelled(),0.1);



    //Setting current speed to 5 m/s
    TC.SetCurrentSpeed(2.5);

    //Waiting for 3 seconds
    usleep(3000000);
    TC.Update();
    
    //Checking if the distance travelled corresponds to the time passed and the current speed
    ASSERT_NEAR(32.5, TC.GetDistanceTravelled(),0.1);

    (*CLOCK).Stop();
}

/*
TEST(TrainControllerDeltaTimeTests, DistanceTravelled2)
{
    TickSource tick_source("07:00:00", std::chrono::milliseconds(2));
    tick_source.Start();

    std::shared_ptr<TickSource> sadPtr = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(sadPtr);
    usleep(1000000);
    TC.Update();

    //Checking no distance has been travelled
    ASSERT_EQ(0, TC.GetDistanceTravelled());

    //Setting current speed to 10 m/s
    TC.SetCurrentSpeed(10);

    //waiting 0.2 seconds
    usleep(900000);
    TC.Update();
    
    //Checking if the distance travelled corresponds to the time passed and the current speed
    ASSERT_NEAR(0.9, TC.GetDistanceTravelled(),0.1);

    tick_source.Stop();
}
*/