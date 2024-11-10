/*****************************************************************************
* @file train_controller_delta_time_tests.cc
*
* @brief Unit testing for Tick SOurce implementation in train controller.
*****************************************************************************/

#include "train_controller.h"
#include <unistd.h>
#include <iomanip>
#include <gtest/gtest.h>



// TEST(TrainControllerDeltaTimeTests, DistanceTravelled1)
// {
//     TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
//     std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
//     train_controller::SoftwareTrainController TC(CLOCK);


//     types::Second elapsed_time0(0);
//     types::Second elapsed_time1(1);
//     types::Second elapsed_time2(2);
//     types::Second elapsed_time3(3);

//     usleep(1000000);
//     TC.UpdateDistanceTravelled(elapsed_time0);

//     //Checking no distance has been travelled
//     EXPECT_DOUBLE_EQ(0, TC.GetDistanceTravelled());


//     //Setting current speed to 10 m/s
//     TC.SetCurrentSpeed(10);

//     TC.UpdateDistanceTravelled(elapsed_time2);

//     //Checking if the distance travelled corresponds to the time passed and the current speed
//     EXPECT_DOUBLE_EQ(20, TC.GetDistanceTravelled());



//     //Setting current speed to 5 m/s
//     TC.SetCurrentSpeed(5);

//     TC.UpdateDistanceTravelled(elapsed_time1);

//     //Checking if the distance travelled corresponds to the time passed and the current speed
//     EXPECT_DOUBLE_EQ(25, TC.GetDistanceTravelled());



//     //Setting current speed to 2.5 m/s
//     TC.SetCurrentSpeed(2.5);

//     //Waiting for 3 seconds
//     TC.UpdateDistanceTravelled(elapsed_time3);

//     //Checking if the distance travelled corresponds to the time passed and the current speed
//     EXPECT_DOUBLE_EQ(32.5, TC.GetDistanceTravelled());
// }


// TEST(TrainControllerDeltaTimeTests, DistanceTravelled2)
// {
//     TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
//     std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
//     train_controller::SoftwareTrainController TC(CLOCK);

//     (*CLOCK).Start();
//     usleep(1000000);
//     TC.Update();

//     //Checking no distance has been travelled
//     ASSERT_EQ(0, TC.GetDistanceTravelled());

//     //Setting current speed to 10 m/s
//     TC.SetCurrentSpeed(10);

//     //waiting 0.9 seconds
//     usleep(900000);
//     TC.Update();

//     //Checking if the distance travelled corresponds to the time passed and the current speed
//     EXPECT_DOUBLE_EQ(9, TC.GetDistanceTravelled());

//     tick_source.Stop();
// }

TEST(TrainControllerDeltaTimeTests, DistanceTravelledSinceLastUpdate)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
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
    EXPECT_DOUBLE_EQ(9, TC.GetDistanceTravelledSinceLastUpdate());
    EXPECT_DOUBLE_EQ(9, TC.GetDistanceTravelled());

    usleep(100000);
    TC.Update();
    EXPECT_DOUBLE_EQ(1, TC.GetDistanceTravelledSinceLastUpdate());
    EXPECT_DOUBLE_EQ(10, TC.GetDistanceTravelled());

    tick_source.Stop();
}
