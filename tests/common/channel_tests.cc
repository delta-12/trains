/*****************************************************************************
 * @file channel_tests.cc
 *
 * @brief Unit testing for Channel.
 *****************************************************************************/

#include <thread>

#include <gtest/gtest.h>

#include "channel.h"

TEST(ChannelTests, SendAndReceive)
{
    Channel<int> channel;

    std::thread worker_thread([&]{ channel.Send(69); });

    ASSERT_EQ(69, channel.Receive());
    worker_thread.join();
}