/*****************************************************************************
* @file controller_handler_tests.h
*
* @brief Unit testing for controller handler.
*****************************************************************************/

#include <gtest/gtest.h>

#include "controller_handler.h"
#include "controller_port.h"
#include "ctc.h"
#include "ring_buffer.h"
#include "types.h"

RingBuffer<uint8_t, 1024> ring_buffer;

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
            return true;
        }
};

TEST(ControllerHandlerTests, IsConnectedTest)
{
    ctc::Ctc                                    ctc_office;
    controller_network::ControllerHandler<1024> controller_handler;
    controller_handler.AddPort(std::make_unique<controller_network::BasicControllerPort<1024>>(std::make_unique<SoftwarePort>()));

    controller_handler.Update(ctc_office);
}