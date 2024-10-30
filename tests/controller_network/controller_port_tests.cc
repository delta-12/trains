/*****************************************************************************
* @file controller_port_tests.h
*
* @brief Unit testing for controller port.
*****************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include "controller_port.h"
#include "ring_buffer.h"
#include "types.h"

class SoftwarePort : public types::Port
{
    public:
        size_t Send(const uint8_t *const buffer, const size_t size)
        {
            return buffer_.Write(buffer, size);
        }
        size_t SendAvailable(void)
        {
            return (buffer_.Capacity() - buffer_.Size());
        }
        size_t Receive(uint8_t *const buffer, const size_t size)
        {
            return buffer_.Read(buffer, size);
        }
        size_t ReceiveAvailable(void)
        {
            return buffer_.Size();
        }
        bool Connected(void)
        {
            return true;
        }

    private:
        RingBuffer<uint8_t, 1024> buffer_;
};

TEST(BasicControllerTests, SendMessageTest)
{
    uint8_t                                       send_data[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0x10};
    controller_network::BasicControllerPort<1024> controller_port(std::make_unique<SoftwarePort>());

    // Nullptr
    ASSERT_EQ(0, controller_port.SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, nullptr, sizeof(send_data)));

    // Size 0
    ASSERT_EQ(0, controller_port.SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, send_data, 0));

    // Nullptr and size 0
    ASSERT_EQ(0, controller_port.SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, nullptr, 0));

    // Insufficient send_buffer_ space
    ASSERT_EQ(0, controller_port.SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, send_data, 1025));

    // Success
    ASSERT_EQ(sizeof(send_data), controller_port.SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, send_data, sizeof(send_data)));

    // Insufficient port buffer space
    while (0 != controller_port.SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, send_data, sizeof(send_data)))
    {
    }
    ASSERT_EQ(0, controller_port.SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, send_data, sizeof(send_data)));
}

TEST(BasicControllerTests, ReceiveMessageTest)
{
    uint8_t                                       send_data[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0x10};
    uint8_t                                       receive_data[sizeof(send_data)];
    controller_network::MessageType               message_type;
    controller_network::BasicControllerPort<1024> controller_port(std::make_unique<SoftwarePort>());

    // Nullptr
    ASSERT_EQ(0, controller_port.ReceiveMessage(message_type, nullptr, sizeof(receive_data)));

    // Size 0
    ASSERT_EQ(0, controller_port.ReceiveMessage(message_type, receive_data, 0));

    // Nullptr and size 0
    ASSERT_EQ(0, controller_port.ReceiveMessage(message_type, nullptr, 0));

    // No message available
    ASSERT_EQ(0, controller_port.ReceiveMessage(message_type, receive_data, sizeof(receive_data)));

    // Payload bigger than buffer
    ASSERT_EQ(sizeof(send_data), controller_port.SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, send_data, sizeof(send_data)));
    ASSERT_EQ(sizeof(send_data) - 1, controller_port.ReceiveMessage(message_type, receive_data, sizeof(receive_data) - 1));

    // Success
    ASSERT_EQ(sizeof(send_data), controller_port.SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, send_data, sizeof(send_data)));
    ASSERT_EQ(sizeof(send_data), controller_port.ReceiveMessage(message_type, receive_data, sizeof(receive_data)));
    ASSERT_THAT(receive_data, testing::ElementsAreArray(send_data));
}

TEST(BasicControllerTests, PortConnectedTest)
{
    controller_network::BasicControllerPort<1024> controller_port(std::make_unique<SoftwarePort>());

    ASSERT_TRUE(controller_port.Connected());
}