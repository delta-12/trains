/*****************************************************************************
* @file controller_handler_tests.h
*
* @brief Unit testing for controller handler.
*****************************************************************************/

#include <gtest/gtest.h>

#include "connection.pb.h"
#include "controller_handler.h"
#include "controller_port.h"
#include "ctc.h"
#include "ring_buffer.h"
#include "types.h"

RingBuffer<uint8_t, 1024> ring_buffer;
bool                      connected = true;

class SoftwarePort : public types::Port
{
    public:
        size_t Send(const uint8_t *const buffer, const size_t size)
        {
            return ring_buffer.Write(buffer, size);
        }
        size_t SendAvailable(void)
        {
            return (ring_buffer.Capacity() - ring_buffer.Size());
        }
        size_t Receive(uint8_t *const buffer, const size_t size)
        {
            return ring_buffer.Read(buffer, size);
        }
        size_t ReceiveAvailable(void)
        {
            return ring_buffer.Size();
        }
        bool Connected(void)
        {
            return connected;
        }
};

TEST(ControllerHandlerTests, IsConnectedTestWayside0)
{
    ctc::Ctc                                                       ctc_office;
    controller_network::ControllerHandler<1024>                    controller_handler;
    std::unique_ptr<controller_network::BasicControllerPort<1024>> client_port = std::make_unique<controller_network::BasicControllerPort<1024>>(std::make_unique<SoftwarePort>());
    controller_handler.AddPort(std::make_unique<controller_network::BasicControllerPort<1024>>(std::make_unique<SoftwarePort>()));

    controller_handler.Update(ctc_office);

    std::array<uint8_t, 1024>       message_buffer;
    controller_messages::Connection connection_message;
    connection_message.set_controller_id(0);
    connection_message.set_controller_type(controller_messages::CONTROLLER_TYPE_WAYSIDE);
    connection_message.SerializeToArray(message_buffer.data(), message_buffer.size());
    client_port->SendMessage(controller_network::MESSAGETYPE_CONNECTION, message_buffer.data(), connection_message.ByteSizeLong());
    controller_handler.Update(ctc_office);
    ASSERT_TRUE(controller_handler.IsControllerConnected(controller_network::CONTROLLERTYPE_WAYSIDE, 0));
    connected = false;
    controller_handler.Update(ctc_office);
    ASSERT_FALSE(controller_handler.IsControllerConnected(controller_network::CONTROLLERTYPE_WAYSIDE, 0));
    connected = true;
}

TEST(ControllerHandlerTests, IsConnectedTestWayside1)
{
    ctc::Ctc                                                       ctc_office;
    controller_network::ControllerHandler<1024>                    controller_handler;
    std::unique_ptr<controller_network::BasicControllerPort<1024>> client_port = std::make_unique<controller_network::BasicControllerPort<1024>>(std::make_unique<SoftwarePort>());
    controller_handler.AddPort(std::make_unique<controller_network::BasicControllerPort<1024>>(std::make_unique<SoftwarePort>()));

    controller_handler.Update(ctc_office);

    std::array<uint8_t, 1024>       message_buffer;
    controller_messages::Connection connection_message;
    connection_message.set_controller_id(1);
    connection_message.set_controller_type(controller_messages::CONTROLLER_TYPE_WAYSIDE);
    connection_message.SerializeToArray(message_buffer.data(), message_buffer.size());
    client_port->SendMessage(controller_network::MESSAGETYPE_CONNECTION, message_buffer.data(), connection_message.ByteSizeLong());
    controller_handler.Update(ctc_office);
    ASSERT_TRUE(controller_handler.IsControllerConnected(controller_network::CONTROLLERTYPE_WAYSIDE, 1));
    connected = false;
    controller_handler.Update(ctc_office);
    ASSERT_FALSE(controller_handler.IsControllerConnected(controller_network::CONTROLLERTYPE_WAYSIDE, 1));
    connected = true;
}

TEST(ControllerHandlerTests, TrackCircuitDataTest)
{
    ctc::Ctc                                                       ctc_office;
    simulator::Simulator                                           world_simulator;
    controller_network::ControllerHandler<1024>                    controller_handler;
    std::unique_ptr<controller_network::BasicControllerPort<1024>> client_port = std::make_unique<controller_network::BasicControllerPort<1024>>(std::make_unique<SoftwarePort>());
    controller_handler.AddPort(std::make_unique<controller_network::BasicControllerPort<1024>>(std::make_unique<SoftwarePort>()));

    controller_handler.Update(ctc_office, world_simulator);

    std::array<uint8_t, 1024>       message_buffer;
    controller_messages::Connection connection_message;
    connection_message.set_controller_id(0);
    connection_message.set_controller_type(controller_messages::CONTROLLER_TYPE_WAYSIDE);
    connection_message.SerializeToArray(message_buffer.data(), message_buffer.size());
    client_port->SendMessage(controller_network::MESSAGETYPE_CONNECTION, message_buffer.data(), connection_message.ByteSizeLong());
    controller_handler.Update(ctc_office, world_simulator);
    ASSERT_TRUE(controller_handler.IsControllerConnected(controller_network::CONTROLLERTYPE_WAYSIDE, 0));
    ASSERT_FALSE(controller_handler.IsControllerConnected(controller_network::CONTROLLERTYPE_MAX, 0));
}