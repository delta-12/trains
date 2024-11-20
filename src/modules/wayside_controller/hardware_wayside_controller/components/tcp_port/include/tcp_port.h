#ifndef TCP_PORT_H
#define TCP_PORT_H

#include <array>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "ring_buffer.h"
#include "types.h"

const size_t kEspTcpPortBufferSize = 1024;
const size_t kEspTcpPortIntermediateBufferSize = 512;

class EspTcpPort : public types::Port
{
public:
    EspTcpPort(void);
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
    static void SendTask(void *arg);
    static void ReceiveTask(void *arg);
    inline bool LockSender(void);
    inline void UnlockSender(void);
    inline bool LockReceiver(void);
    inline void UnlockReceiver(void);

    int socket_;
    bool connected_ = false;
    std::array<uint8_t, kEspTcpPortIntermediateBufferSize> receive_buffer_;
    std::array<uint8_t, kEspTcpPortIntermediateBufferSize> send_buffer_;
    RingBuffer<uint8_t, kEspTcpPortBufferSize> receiver_ring_buffer_;
    RingBuffer<uint8_t, kEspTcpPortBufferSize> sender_ring_buffer_;
    SemaphoreHandle_t receiver_mutex_ = NULL;
    StaticSemaphore_t receiver_mutex_buffer_;
    SemaphoreHandle_t sender_mutex_ = NULL;
    StaticSemaphore_t sender_mutex_buffer_;
};

#endif // TCP_PORT_H