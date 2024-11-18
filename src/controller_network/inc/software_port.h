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

template<template<typename, size_t> class RingBuffer, typename T, size_t buffer_size>
class SoftwarePort : public types::Port
{
    public:
        SoftwarePort(RingBuffer<T, buffer_size> &send_buffer, RingBuffer<T, buffer_size> &receive_buffer);
        size_t Send(const uint8_t *const buffer, const size_t size);
        size_t SendAvailable(void);
        size_t Receive(uint8_t *const buffer, const size_t size);
        size_t ReceiveAvailable(void);
        bool Connected(void);

    private:
        RingBuffer<T, buffer_size> &send_buffer_;
        RingBuffer<T, buffer_size> &receive_buffer_;
};

template<template<typename, size_t> class RingBuffer, typename T, size_t buffer_size>
SoftwarePort<RingBuffer, T, buffer_size>::SoftwarePort(RingBuffer<T, buffer_size> &send_buffer, RingBuffer<T, buffer_size> &receive_buffer)
    : send_buffer_(send_buffer), receive_buffer_(receive_buffer)
{
}

template<template<typename, size_t> class RingBuffer, typename T, size_t buffer_size>
size_t SoftwarePort<RingBuffer, T, buffer_size>::Send(const uint8_t *const buffer, const size_t size)
{
    return send_buffer_.Write(buffer, size);
}

template<template<typename, size_t> class RingBuffer, typename T, size_t buffer_size>
size_t SoftwarePort<RingBuffer, T, buffer_size>::SendAvailable(void)
{
    return (send_buffer_.Capacity() - send_buffer_.Size());
}

template<template<typename, size_t> class RingBuffer, typename T, size_t buffer_size>
size_t SoftwarePort<RingBuffer, T, buffer_size>::Receive(uint8_t *const buffer, const size_t size)
{
    return receive_buffer_.Read(buffer, size);
}

template<template<typename, size_t> class RingBuffer, typename T, size_t buffer_size>
size_t SoftwarePort<RingBuffer, T, buffer_size>::ReceiveAvailable(void)
{
    return receive_buffer_.Size();
}

template<template<typename, size_t> class RingBuffer, typename T, size_t buffer_size>
bool SoftwarePort<RingBuffer, T, buffer_size>::Connected(void)
{
    return true;
}

} // namespace controller_network

#endif // TRAINS_SRC_CONTROLLER_NETWORK_INC_SOFTWARE_PORT_H