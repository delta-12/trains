/*****************************************************************************
* @file train_model_passenger_count_tests.cc
*
* @brief Unit testing for passenger counting functions
*****************************************************************************/

#include "train_model.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <gtest/gtest.h>



TEST(TrainModelPassengerCountTests, TrainID1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    EXPECT_DOUBLE_EQ(0, TM.GetTrainId());

    TM.SetTrainId(1);

    EXPECT_DOUBLE_EQ(1, TM.GetTrainId());
}