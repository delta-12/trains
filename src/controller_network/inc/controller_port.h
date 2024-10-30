/*****************************************************************************
* @file controller_port.h
*
* @brief Implements the Controller Message Protocol.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_CONTROLLER_PORT_H
#define TRAINS_SRC_COMMON_INC_CONTROLLER_PORT_H

#include <algorithm>
#include <array>
#include <functional>
#include <memory>

#include "controller_network_protocols.h"
#include "convert.h"
#include "types.h"

namespace controller_network
{

class ControllerPort
{
    public:
        virtual size_t SendMessage(const MessageType type, const uint8_t *const message, const uint16_t size) = 0;
        virtual size_t ReceiveMessage(MessageType &type, uint8_t *const message, const uint16_t size)         = 0;
        virtual bool Connected(void) const                                                                    = 0;
};

template <size_t buffer_size>
class BasicControllerPort : public ControllerPort
{
    public:
        BasicControllerPort(std::unique_ptr<types::Port> port);
        BasicControllerPort(const BasicControllerPort &)            = delete;
        BasicControllerPort &operator=(const BasicControllerPort &) = delete;
        BasicControllerPort(BasicControllerPort &&)                 = delete;
        BasicControllerPort &operator=(BasicControllerPort &&)      = delete;
        size_t SendMessage(const MessageType type, const uint8_t *const message, const uint16_t size);
        size_t ReceiveMessage(MessageType &type, uint8_t *const message, const uint16_t size);
        bool Connected(void) const;

    private:
        std::unique_ptr<types::Port> port_;
        std::array<uint8_t, buffer_size> send_buffer_;
        std::array<uint8_t, buffer_size> receive_buffer_;
        size_t bytes_received_;
};

template <size_t buffer_size>
BasicControllerPort<buffer_size>::BasicControllerPort(std::unique_ptr<types::Port> port) : port_(std::move(port)), bytes_received_(0)
{
}

template <size_t buffer_size>
size_t BasicControllerPort<buffer_size>::SendMessage(const MessageType type, const uint8_t *const message, const uint16_t size)
{
    size_t message_bytes_sent = 0;
    size_t message_size       = MESSAGEHEADERBYTE_HEADER_MAX + size;

    if ((nullptr == message) || (0 == size))
    {
        // No message to send, do nothing
    }
    else if ((send_buffer_.size() < message_size) || (port_->SendAvailable() < message_size))
    {
        // Not enough space, do nothing
    }
    else
    {
        send_buffer_[MESSAGEHEADERBYTE_VERSION]           = MESSAGEPROTOCOLVERSION_0;
        send_buffer_[MESSAGEHEADERBYTE_TYPE]              = type;
        send_buffer_[MESSAGEHEADERBYTE_PAYLOAD_LENGTH_HI] = convert::GetHighByte(size);
        send_buffer_[MESSAGEHEADERBYTE_PAYLOAD_LENGTH_LO] = convert::GetLowByte(size);

        std::copy(message, (message + size), (std::begin(send_buffer_) + MESSAGEHEADERBYTE_HEADER_MAX));

        if (port_->Send(send_buffer_.data(), message_size) == message_size)
        {
            message_bytes_sent = size;
        }
    }

    return message_bytes_sent;
}

template <size_t buffer_size>
size_t BasicControllerPort<buffer_size>::ReceiveMessage(MessageType &type, uint8_t *const message, const uint16_t size)
{
    size_t message_bytes_received = 0;

    // Receive message header
    if (bytes_received_ < MESSAGEHEADERBYTE_HEADER_MAX)
    {
        bytes_received_ += port_->Receive((receive_buffer_.data() + bytes_received_), (MESSAGEHEADERBYTE_HEADER_MAX - bytes_received_));
    }

    size_t payload_size = convert::MakeUint16(receive_buffer_[MESSAGEHEADERBYTE_PAYLOAD_LENGTH_HI], receive_buffer_[MESSAGEHEADERBYTE_PAYLOAD_LENGTH_LO]);
    size_t message_size = MESSAGEHEADERBYTE_HEADER_MAX + payload_size;

    // Receive message payload
    if (bytes_received_ >= MESSAGEHEADERBYTE_HEADER_MAX)
    {
        bytes_received_ += port_->Receive((receive_buffer_.data() + bytes_received_), (message_size - bytes_received_));
    }

    // Copy payload to message buffer
    if ((nullptr != message) && (bytes_received_ >= message_size))
    {
        type                   = static_cast<MessageType>(receive_buffer_[MESSAGEHEADERBYTE_TYPE]);
        message_bytes_received = payload_size;

        if (size < payload_size)
        {
            message_bytes_received = size;
        }

        std::copy((receive_buffer_.begin() + MESSAGEHEADERBYTE_HEADER_MAX), (receive_buffer_.begin() + bytes_received_), message);

        bytes_received_ = 0;
    }

    return message_bytes_received;
}

template <size_t buffer_size>
bool BasicControllerPort<buffer_size>::Connected(void) const
{
    return port_->Connected();
}

} // namespace controller_network

#endif // TRAINS_SRC_COMMON_INC_CONTROLLER_PORT_H