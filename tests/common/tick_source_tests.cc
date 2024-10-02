/*****************************************************************************
 * @file tick_source_tests.cc
 *
 * @brief Unit testing for TickSource.
 *****************************************************************************/

#include <unistd.h>

#include <iostream>
#include <iomanip>

#include <gtest/gtest.h>

#include "tick_source.h"

TEST(TickSourceTests, Initialize)
{
    TickSource tick_source("07:00:00", std::chrono::milliseconds(2));

    tick_source.Start();
    usleep(5e5);
    tick_source.Stop();
    ASSERT_EQ(std::chrono::milliseconds(2), tick_source.GetTickDuration());
    ASSERT_EQ(250, tick_source.GetTick());

    tick_source.Start();
    usleep(5e5);
    tick_source.Stop();
    ASSERT_EQ(500, tick_source.GetTick());

    usleep(1e4);
    ASSERT_EQ(500, tick_source.GetTick());

    tick_source.Start();
    usleep(1e6);
    tick_source.Stop();
    ASSERT_EQ(1000, tick_source.GetTick());
}

TEST(TickSourceTests, MultiplierAndDuration)
{
    TickSource tick_source;

    tick_source.Start();
    usleep(1e4);
    ASSERT_EQ(10, tick_source.GetTick());
    tick_source.SetMultiplier(50);
    usleep(1e4);
    ASSERT_EQ(510, tick_source.GetTick());
    tick_source.SetMultiplier(1);
    usleep(1e4);
    ASSERT_EQ(520, tick_source.GetTick());

    tick_source.SetTickDuration(std::chrono::milliseconds(5));
    usleep(1e4);
    ASSERT_EQ(522, tick_source.GetTick());
    tick_source.SetMultiplier(10);
    usleep(1e4);
    ASSERT_EQ(542, tick_source.GetTick());

    tick_source.SetMultiplier(1);
    tick_source.SetTickDuration(std::chrono::milliseconds(1));
    usleep(1e4);
    ASSERT_EQ(552, tick_source.GetTick());
}

// TEST(TickSourceTests, SetTime)
// {
//     TickSource tick_source("07:00:00", std::chrono::milliseconds(1));

//     tick_source.SetMultiplier(100);
//     tick_source.Start();
//     usleep(1e4);
//     ASSERT_EQ(1000, tick_source.GetTick());
//     ASSERT_STREQ("07:00:01", tick_source.GetTimeString().c_str());
//     tick_source.SetTime("07:30:00");
//     usleep(2e4);
//     ASSERT_EQ(2000, tick_source.GetTick());
//     ASSERT_STREQ("07:30:02", tick_source.GetTimeString().c_str());
//     tick_source.SetTime("12:59:59");
//     usleep(1e4);
//     ASSERT_EQ(1000, tick_source.GetTick());
//     ASSERT_STREQ("13:00:00", tick_source.GetTimeString().c_str());
// }

TEST(TickSourceTests, GetElapsedTime)
{
    TickSource tick_source;

    tick_source.Start();
    usleep(1e4);
    types::Tick start = tick_source.GetTick();
    usleep(3e4);
    ASSERT_EQ(30, tick_source.GetElapsedTime(start).count());
    usleep(1e4);
    types::Tick end = tick_source.GetTick();
    ASSERT_EQ(40, tick_source.GetElapsedTime(start, end).count());
}