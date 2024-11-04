/*****************************************************************************
* @file train_controller_authority_tests.cc
*
* @brief Unit testing for authority implementation in train controller.
*****************************************************************************/

#include "train_controller.h"
#include "train_model.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <gtest/gtest.h>



TEST(TrainControllerAuthorityTests, PowerWithAuthority)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();

    TC.SetAuthority(1);
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(18);

    usleep(1000000);
    TC.Update();

    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetDistanceTravelled(),0);


    TC.SetCurrentSpeed(10);

    usleep(10000000);
    TC.Update();

    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n";
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);
}
