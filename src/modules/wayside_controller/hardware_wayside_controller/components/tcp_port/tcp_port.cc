#include "tcp_port.h"

#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h> // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"

static const char *const kLogTag = "TCP PORT";

EspTcpPort::EspTcpPort(const char *const host_ip, const uint16_t port_number)
{
    receiver_mutex_ = xSemaphoreCreateMutexStatic(&receiver_mutex_buffer_);
    sender_mutex_ = xSemaphoreCreateMutexStatic(&sender_mutex_buffer_);

    Connect(host_ip, port_number);
}

EspTcpPort::~EspTcpPort(void)
{
    if (connected_)
    {
        // TODO stop update task

        Close();
    }
}

size_t EspTcpPort::Send(const uint8_t *const buffer, const size_t size)
{
    size_t bytes_sent = 0;

    if (LockSender())
    {
        bytes_sent = sender_ring_buffer_.Write(buffer, size);

        UnlockSender();
    }

    return bytes_sent;
}

size_t EspTcpPort::SendAvailable(void)
{
    size_t bytes_available = 0;

    if (LockSender())
    {
        bytes_available = sender_ring_buffer_.Capacity() - sender_ring_buffer_.Size();

        UnlockSender();
    }

    return bytes_available;
}

size_t EspTcpPort::Receive(uint8_t *const buffer, const size_t size)
{
    size_t bytes_received = 0;

    if (LockReceiver())
    {
        bytes_received = receiver_ring_buffer_.Read(buffer, size);

        UnlockReceiver();
    }

    return bytes_received;
}

size_t EspTcpPort::ReceiveAvailable(void)
{
    size_t bytes_available = 0;

    if (LockReceiver())
    {
        bytes_available = receiver_ring_buffer_.Size();

        UnlockReceiver();
    }

    return bytes_available;
}

bool EspTcpPort::Connected(void)
{
    return connected_;
}

void EspTcpPort::Connect(const char *const host_ip, const uint16_t port_number)
{
    struct sockaddr_in destination_address;
    inet_pton(AF_INET, host_ip, &destination_address.sin_addr);
    destination_address.sin_family = AF_INET;
    destination_address.sin_port = htons(port_number);

    int socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (socket_ < 0)
    {
        ESP_LOGE(kLogTag, "Unable to create socket: errno %d", errno);

        Close();
    }
    else
    {
        ESP_LOGI(kLogTag, "Socket created, connecting to %s:%d", host_ip, port_number);

        if (0 != connect(socket_, (struct sockaddr *)&destination_address, sizeof(destination_address)))
        {
            ESP_LOGE(kLogTag, "Socket unable to connect: errno %d", errno);
        }
        else
        {
            ESP_LOGI(kLogTag, "Successfully connected");

            connected_ = true;

            // TODO start update task
        }
    }
}

void EspTcpPort::Close(void)
{
    shutdown(socket_, 0);
    close(socket_);
}

void EspTcpPort::SendTask(void)
{
    // TODO
}

void EspTcpPort::ReceiveTask(void)
{
    // TODO
}

inline bool EspTcpPort::LockSender(void)
{
    bool locked = false;

    if (xSemaphoreTake(sender_mutex_, portMAX_DELAY) == pdTRUE)
    {
        locked = true;
    }

    return locked;
}

inline void EspTcpPort::UnlockSender(void)
{
    xSemaphoreGive(sender_mutex_);
}

inline bool EspTcpPort::LockReceiver(void)
{
    bool locked = false;

    if (xSemaphoreTake(receiver_mutex_, portMAX_DELAY) == pdTRUE)
    {
        locked = true;
    }

    return locked;
}

inline void EspTcpPort::UnlockReceiver(void)
{
    xSemaphoreGive(receiver_mutex_);
}