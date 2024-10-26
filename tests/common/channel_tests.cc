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

    ASSERT_FALSE(channel.DataAvailable());
    std::thread worker_thread([&]{
                              channel.Send(69);
        });

    while (!channel.DataAvailable())
    {
    }

    ASSERT_TRUE(channel.DataAvailable());
    ASSERT_EQ(69, channel.Receive());
    ASSERT_FALSE(channel.DataAvailable());
    worker_thread.join();
}