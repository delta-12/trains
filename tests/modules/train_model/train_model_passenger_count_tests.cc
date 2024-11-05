/*****************************************************************************
* @file train_model_passenger_count_tests.cc
*
* @brief Unit testing for passenger counting functions
*****************************************************************************/

#include "train_model.h"
#include <unistd.h>
#include <iomanip>
#include <gtest/gtest.h>



TEST(TrainModelPassengerCountTests, PassengerCount1)
{
    TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
    train_model::SoftwareTrainModel TM(CLOCK);

    types::Second elapsed_time0(0);
    types::Second elapsed_time1(1);
    types::Second elapsed_time2(2);
    types::Second elapsed_time3(3);

    EXPECT_DOUBLE_EQ(0, TM.GetPassengersCount());

    TM.SetPassengersBoarding(30);//will be done by track model
    //updates passenger count

    EXPECT_DOUBLE_EQ(30, TM.GetPassengersCount());

    TM.SetPassengersBoarding(40);//will be done by track model
    //updates passenger count

    bool random_range = (TM.GetPassengersCount() >= 40 && TM.GetPassengersCount() <= 70);

    EXPECT_DOUBLE_EQ(random_range, true);
}