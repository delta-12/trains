/*****************************************************************************
* @file train_model_basic_set_get_tests.cc
*
* @brief Unit testing for passenger counting functions
*****************************************************************************/

#include "train_model.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <gtest/gtest.h>



TEST(TrainModelBasicSetGetTests, BrakeFailure1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(false, TM.GetBrakeFailure());

    TM.SetBrakeFailure(true);

    EXPECT_DOUBLE_EQ(true, TM.GetBrakeFailure());
}
TEST(TrainModelBasicSetGetTests, EngineFailure1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(false, TM.GetEngineFailure());

    TM.SetEngineFailure(true);

    EXPECT_DOUBLE_EQ(true, TM.GetEngineFailure());
}
TEST(TrainModelBasicSetGetTests, SignalPickupFailure1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(false, TM.GetSignalPickupFailure());

    TM.SetSignalPickupFailure(true);

    EXPECT_DOUBLE_EQ(true, TM.GetSignalPickupFailure());
}
TEST(TrainModelBasicSetGetTests, Authority1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(0, TM.GetAuthority());

    TM.SetAuthority(40);

    EXPECT_DOUBLE_EQ(40, TM.GetAuthority());
}
TEST(TrainModelBasicSetGetTests, ActualSpeed1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(0, TM.GetActualSpeed());
}
TEST(TrainModelBasicSetGetTests, CommandedSpeed1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(0, TM.GetCommandedSpeed());

    TM.SetCommandedSpeed(55);

    EXPECT_DOUBLE_EQ(55, TM.GetCommandedSpeed());
}
TEST(TrainModelBasicSetGetTests, Power1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(0, TM.GetActualPower());

    TM.SetCommandedPower(6000);

    EXPECT_DOUBLE_EQ(6000, TM.GetActualPower());
}
TEST(TrainModelBasicSetGetTests, InternalTemperature1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(68, TM.GetActualInternalTemperature());

    TM.SetCommandedInternalTemperature(72);

    EXPECT_DOUBLE_EQ(72, TM.GetActualInternalTemperature());
}
TEST(TrainModelBasicSetGetTests, DistanceTraveled1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(0, TM.GetDistanceTraveled());

    TM.SetDistanceTraveled(120);

    EXPECT_DOUBLE_EQ(120, TM.GetDistanceTraveled());
}
//TEST(TrainModelBasicSetGetTests, TrackPolarity1)
//{
//    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
//    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
//    train_model::SoftwareTrainModel TM(CLOCK);
//
//    EXPECT_DOUBLE_EQ(0, TM.GetTrackPolarity());
//
//    TM.SetTrackPolarity(1);
//
//    EXPECT_DOUBLE_EQ(1, TM.GetTrackPolarity());
//}