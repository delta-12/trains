/*****************************************************************************
* @file software_port.h
*
* @brief Implements a software port.
*****************************************************************************/
#ifndef TRAINS_SRC_CONTROLLER_NETWORK_INC_SOFTWARE_PORT_H
#define TRAINS_SRC_CONTROLLER_NETWORK_INC_SOFTWARE_PORT_H

#include "ring_buffer.h"
#include "types.h"

namespace controller_network
{

template<typename T, size_t buffer_size>
class SoftwarePort : public types::Port
{
    public:
        SoftwarePort(RingBuffer<T, buffer_size> &ring_buffer);
        size_t Send(const uint8_t *const buffer, const size_t size);
        size_t SendAvailable(void);
        size_t Receive(uint8_t *const buffer, const size_t size);
        size_t ReceiveAvailable(void);
        bool Connected(void);

    private:
        RingBuffer<T, buffer_size> &buffer_;
};

template<typename T, size_t buffer_size>
SoftwarePort<T, buffer_size>::SoftwarePort(RingBuffer<T, buffer_size> &ring_buffer) : buffer_(ring_buffer)
{
}

template<typename T, size_t buffer_size>
size_t SoftwarePort<T, buffer_size>::Send(const uint8_t *const buffer, const size_t size)
{
    return buffer_.Write(buffer, size);
}

template<typename T, size_t buffer_size>
size_t SoftwarePort<T, buffer_size>::SendAvailable(void)
{
    return (buffer_.Capacity() - buffer_.Size());
}

template<typename T, size_t buffer_size>
size_t SoftwarePort<T, buffer_size>::Receive(uint8_t *const buffer, const size_t size)
{
    return buffer_.Read(buffer, size);
}

template<typename T, size_t buffer_size>
size_t SoftwarePort<T, buffer_size>::ReceiveAvailable(void)
{
    return buffer_.Size();
}

template<typename T, size_t buffer_size>
bool SoftwarePort<T, buffer_size>::Connected(void)
{
    return true;
}

} // namespace controller_network

#endif // TRAINS_SRC_CONTROLLER_NETWORK_INC_SOFTWARE_PORT_H