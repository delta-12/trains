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
    TickSource tick_source("07:00:00", std::chrono::milliseconds(2));
    tick_source.Start();

    std::shared_ptr<TickSource> sadPtr = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(sadPtr);
    usleep(1000000);
    TC.Update();

    //Checking if the delta time corresponds to the 1 second wait time
    ASSERT_EQ(1, TC.GetDeltaTime());

    usleep(2000000);
    TC.Update();
    
    //Checking if the delta time corresponds to the 2 second wait time
    ASSERT_EQ(2, TC.GetDeltaTime());

    tick_source.Stop(); 
}
