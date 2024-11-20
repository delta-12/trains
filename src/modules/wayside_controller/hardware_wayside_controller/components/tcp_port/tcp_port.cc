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
    // TODO lock buffer
    return sender_ring_buffer_.Write(buffer, size);
    // TODO unlock buffer
}

size_t EspTcpPort::SendAvailable(void)
{
    // TODO lock buffer
    return sender_ring_buffer_.Size();
    // TODO unlock buffer
}

size_t EspTcpPort::Receive(uint8_t *const buffer, const size_t size)
{
    // TODO lock buffer
    return receiver_ring_buffer_.Read(buffer, size);
    // TODO unlock buffer
}

size_t EspTcpPort::ReceiveAvailable(void)
{
    // TODO lock buffer
    return receiver_ring_buffer_.Size();
    // TODO unlock buffer
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

void EspTcpPort::Update(void)
{
    // TODO
}