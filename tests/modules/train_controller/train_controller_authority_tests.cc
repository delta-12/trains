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
    ASSERT_EQ(TC.GetDistanceTravelled(), 0);


    TC.SetCurrentSpeed(10);

    usleep(5000000);
    TC.Update();

    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n";
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);

    TC.SetAuthority(0);
    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);
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
    ASSERT_EQ(TC.GetDistanceTravelled(), 0);


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
    ASSERT_EQ(TC.GetDistanceTravelled(), 0);



    TC.SetCurrentSpeed(10);
    usleep(5000000);
    TC.Update();
    TC.SetAuthority(4);
    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(5000000);
    TC.Update();
    TC.SetAuthority(3);
    TC.SetPolartity(types::Polarity::POLARITY_NEGATIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(10000000);
    TC.Update();
    TC.SetAuthority(2);
    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);
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



TEST(TrainControllerAuthorityTests, ChangingAuthorityToALargerValueBeforeSlowingDown)
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
    ASSERT_EQ(TC.GetDistanceTravelled(), 0);


    TC.SetCurrentSpeed(10);
    usleep(5000000);
    TC.Update();
    TC.SetAuthority(4);
    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(5000000);
    TC.Update();
    TC.SetAuthority(5);
    TC.SetPolartity(types::Polarity::POLARITY_NEGATIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(10000000);
    TC.Update();
    TC.SetAuthority(4);
    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(10000000);
    TC.Update();
    TC.SetAuthority(3);
    TC.SetPolartity(types::Polarity::POLARITY_NEGATIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);


    usleep(5000000);
    TC.Update();
    TC.SetAuthority(2);
    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);
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



TEST(TrainControllerAuthorityTests, ChangingAuthorityToASmallerValueBeforeSlowingDown)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();
    (*CLOCK).SetMultiplier(2);

    // IN Block 63
    TC.SetAuthority(5);
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(18);
    usleep(500000);
    TC.Update();
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetDistanceTravelled(), 0);
    TC.SetCurrentSpeed(10);
    usleep(5000000);
    TC.Update();

    // IN Block 64
    TC.SetAuthority(4);
    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);
    std::cout <<  "======================\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Authority: " << TC.GetAuthority() << "\n";
    usleep(5000000);
    TC.Update();

    // IN Block 65

    TC.SetAuthority(1);
    TC.SetPolartity(types::Polarity::POLARITY_NEGATIVE);
    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);
    std::cout <<  "======================\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Authority: " << TC.GetAuthority() << "\n";
    usleep(10000000);
    TC.Update();

    TC.SetAuthority(0);
    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);

    std::cout <<  "======================\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Authority: " << TC.GetAuthority() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n======================";
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);
}

TEST(TrainControllerAuthorityTests, CheckingThatCalculatedServiceBrakeLeadsToStoppingAtCorrectPoint)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();
    (*CLOCK).SetMultiplier(4);

    TC.SetAuthority(1);
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(18);

    usleep(500000);
    TC.Update();

    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetDistanceTravelled(), 0);


    TC.SetCurrentSpeed(10);

    usleep(2500000);

    TC.Update();

    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n";
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);



    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);
    TC.SetAuthority(0);

    // TrainModel recieves values

    // Calculates it (Updates)

    // We Get values from train model (Updated Speed)


    types::Tick last_tick_updated_;
    last_tick_updated_ = (*CLOCK).GetTick();
    types::Second             delta_time;
    double                    total = 0;
    std::chrono::milliseconds elapsed_time;

    while (convert::MilesPerHourToMetersPerSecond(TC.GetCurrentSpeed()) != 0)
    {
        elapsed_time       = (*CLOCK).GetElapsedTime(last_tick_updated_);
        delta_time         = std::chrono::duration_cast<types::Second>(elapsed_time);
        last_tick_updated_ = (*CLOCK).GetTick();

        total += delta_time.count();
        double deceleration = TC.GetServiceBrake() * -1.2;
        double newSpeed     = convert::MilesPerHourToMetersPerSecond(TC.GetCurrentSpeed()) + deceleration * delta_time.count();

        if (newSpeed < 0)
        {
            newSpeed = 0;
        }

        TC.SetCurrentSpeed(newSpeed);
        TC.Update();

        usleep(1000);
    }

    // std::cout <<  "\n Time Passed Before Stopping: "<< total << "\n";
    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n";

    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);
    ASSERT_NEAR(TC.GetDistanceTravelled(), 150, 2);
}

//A test that checks if authority udpdates correctly when the service brakes are triggered. Higher Authority
TEST(TrainControllerAuthorityTests, IncreasingAuthorityWhileSlowingDown)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();
    (*CLOCK).SetMultiplier(4);

    TC.SetAuthority(1);
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(18);

    usleep(500000);
    TC.Update();

    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetDistanceTravelled(), 0);


    TC.SetCurrentSpeed(10);

    usleep(2500000);
    TC.Update();

    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n";
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);

    TC.SetPolartity(types::Polarity::POLARITY_POSITIVE);
    TC.SetAuthority(0);


    types::Tick last_tick_updated_;
    last_tick_updated_ = (*CLOCK).GetTick();
    types::Second             delta_time;
    double                    total = 0;
    std::chrono::milliseconds elapsed_time;

    while (convert::MilesPerHourToMetersPerSecond(TC.GetCurrentSpeed()) > 5)
    {
        elapsed_time       = (*CLOCK).GetElapsedTime(last_tick_updated_);
        delta_time         = std::chrono::duration_cast<types::Second>(elapsed_time);
        last_tick_updated_ = (*CLOCK).GetTick();

        total += delta_time.count();
        double deceleration = TC.GetServiceBrake() * -1.2;
        double newSpeed     = convert::MilesPerHourToMetersPerSecond(TC.GetCurrentSpeed()) + deceleration * delta_time.count();

        if (newSpeed < 0)
        {
            newSpeed = 0;
        }

        TC.SetCurrentSpeed(newSpeed);
        TC.Update();

        usleep(1000);
    }

    std::cout <<  "============================\n Time Passed Before Stopping: " << total << "\n";
    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n============================\n";

    TC.SetAuthority(2);
    usleep(1000);
    TC.Update();

    std::cout <<  "============================\n";
    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n============================\n";

    ASSERT_GT(TC.GetCommandedPower(), 0);
    ASSERT_EQ(TC.GetServiceBrake(), 0);
}

// A test that checks if authority udpdates correctly when the service brakes are triggered. Lower Authority
TEST(TrainControllerAuthorityTests, DecreasingAuthorityWhileSlowingDown)
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
    ASSERT_EQ(TC.GetDistanceTravelled(), 0);


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

    double firstSB = TC.GetServiceBrake();
    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), 0);

    TC.SetAuthority(1);
    usleep(1000);
    TC.Update();

    std::cout <<  "\n Distance Travelled: " << TC.GetDistanceTravelled() << "\n";
    std::cout <<  "\n Commanded Power: " << TC.GetCommandedPower() << "\n";
    std::cout <<  "\n Service Brake: " << TC.GetServiceBrake() << "\n";

    ASSERT_EQ(TC.GetCommandedPower(), 0);
    ASSERT_GT(TC.GetServiceBrake(), firstSB);
}

// Test an Authority that loops back to the beginning of the route
TEST(TrainControllerAuthorityTests, UsingAuthorityThatLoopsAround)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();
    (*CLOCK).SetMultiplier(2);

    TC.SetAuthority(350); //Authority that will have it stop at the first block of the rout. Block 63
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(18);

    usleep(1000);
    TC.Update();

    ASSERT_EQ(TC.GetDistanceOfAuthorityInMeters(), 40105.2);
}