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


TEST(TrainControllerAuthorityTests, UsingAuthorityToStartSlowingDown1Block)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();
    (*CLOCK).SetMultiplier(2);

    TC.SetAuthority(1);
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(18);

    usleep(500000);
    TC.Update();

    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetDistanceTravelled(),0);


    TC.SetCurrentSpeed(10);

    usleep(5000000);
    TC.Update();

    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n";
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);
}


TEST(TrainControllerAuthorityTests, UsingAuthorityThatHasBlocksWithDifferentLengths)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();
    (*CLOCK).SetMultiplier(2);

    TC.SetAuthority(2);
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(18);

    usleep(500000);
    TC.Update();

    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetDistanceTravelled(),0);


    TC.SetCurrentSpeed(10);

    usleep(3000000);
    TC.Update();

    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n";

    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);

    usleep(200000);
    TC.Update();

    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n";

    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);
}


TEST(TrainControllerAuthorityTests, TestingChangingBlocksWithAuthority)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();
    (*CLOCK).SetMultiplier(2);

    TC.SetAuthority(5);
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(18);
    usleep(500000);
    TC.Update();
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetDistanceTravelled(),0);



    TC.SetCurrentSpeed(10);
    usleep(5000000);
    TC.Update();
    TC.SetAuthority(4);
    TC.SetPolartity(types::POLARITY_POSITIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(5000000);
    TC.Update();
    TC.SetAuthority(3);
    TC.SetPolartity(types::POLARITY_NEGATIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);
    

    usleep(10000000);
    TC.Update();
    TC.SetAuthority(2);
    TC.SetPolartity(types::POLARITY_POSITIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);

    usleep(6000000);
    TC.Update();
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);


    std::cout <<  "======================\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Authority: " << TC.GetAuthority() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n======================";
}



TEST(TrainControllerAuthorityTests, TestingChangingAuthorityHalfway)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();
    (*CLOCK).SetMultiplier(2);

    TC.SetAuthority(5);
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(18);
    usleep(500000);
    TC.Update();
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetDistanceTravelled(),0);


    TC.SetCurrentSpeed(10);
    usleep(5000000);
    TC.Update();
    TC.SetAuthority(4);
    TC.SetPolartity(types::POLARITY_POSITIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(5000000);
    TC.Update();
    TC.SetAuthority(5);
    TC.SetPolartity(types::POLARITY_NEGATIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(10000000);
    TC.Update();
    TC.SetAuthority(4);
    TC.SetPolartity(types::POLARITY_POSITIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(10000000);
    TC.Update();
    TC.SetAuthority(3);
    TC.SetPolartity(types::POLARITY_NEGATIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(5000000);
    TC.Update();
    TC.SetAuthority(2);
    TC.SetPolartity(types::POLARITY_POSITIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);



    usleep(1000000);
    TC.Update();
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);

    std::cout <<  "======================\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Authority: " << TC.GetAuthority() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n======================";
}