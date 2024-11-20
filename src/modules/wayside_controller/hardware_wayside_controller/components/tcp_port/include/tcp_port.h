#ifndef TCP_PORT_H
#define TCP_PORT_H

#include <array>

#include "ring_buffer.h"
#include "types.h"

const size_t kEspTcpPortBufferSize = 2048;
const size_t kEspTcpPortIntermediateBufferSize = 512;

class EspTcpPort : public types::Port
{
public:
    EspTcpPort(const char *const host_ip, const uint16_t port_number);
    ~EspTcpPort(void);
    void Connect(const char *const host_ip, const uint16_t port_number);
    size_t Send(const uint8_t *const buffer, const size_t size);
    size_t SendAvailable(void);
    size_t Receive(uint8_t *const buffer, const size_t size);
    size_t ReceiveAvailable(void);
    bool Connected(void);

private:
    void Close(void);
    void Update(void);

    // TODO start task for send/receive
    // TODO add mutex to send/receive ring buffers

    int socket_;
    bool connected_ = false;
    std::array<uint8_t, kEspTcpPortIntermediateBufferSize> receive_buffer_;
    RingBuffer<uint8_t, kEspTcpPortBufferSize> receiver_ring_buffer_;
    RingBuffer<uint8_t, kEspTcpPortBufferSize> sender_ring_buffer_;
};

#endif // TCP_PORT_H