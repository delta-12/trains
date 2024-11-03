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

    TC.SetAuthority(5);
    TC.SetCurrentSpeed(0);
    TC.SetCommandedSpeed(19);
    usleep(1000000);
    TC.Update();

    std::cout << TC.GetCommandedPower();
}
