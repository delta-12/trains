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
static const size_t kTaskStackDepth = 1024;
static const size_t kTaskPriority = 10;

EspTcpPort::EspTcpPort(void)
{
    receiver_mutex_ = xSemaphoreCreateMutexStatic(&receiver_mutex_buffer_);
    sender_mutex_ = xSemaphoreCreateMutexStatic(&sender_mutex_buffer_);
}

EspTcpPort::~EspTcpPort(void)
{
    if (connected_)
    {
        connected_ = false;

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
    if (!connected_)
    {
        struct sockaddr_in destination_address;
        inet_pton(AF_INET, host_ip, &destination_address.sin_addr);
        destination_address.sin_family = AF_INET;
        destination_address.sin_port = htons(port_number);

        socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

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

                xTaskCreate(SendTask, "TcpPortSendTask", kTaskStackDepth, this, kTaskPriority, NULL);
                xTaskCreate(ReceiveTask, "TcpPortReceiveTask", kTaskStackDepth, this, kTaskPriority, NULL);
            }
        }
    }
}

void EspTcpPort::Close(void)
{
    shutdown(socket_, 0);
    close(socket_);
}

void EspTcpPort::SendTask(void *arg)
{
    EspTcpPort *port = (EspTcpPort *)arg;

    while (port->connected_)
    {
        size_t bytes = port->SendAvailable();

        if (bytes > port->send_buffer_.max_size())
        {
            bytes = port->send_buffer_.max_size();
        }

        if (0 == bytes)
        {
        }
        else if (!port->LockSender())
        {
        }
        else
        {
            bytes = port->sender_ring_buffer_.Read(port->send_buffer_.data(), bytes);

            port->UnlockSender();

            if (send(port->socket_, port->send_buffer_.data(), bytes, 0) < 0)
            {
                ESP_LOGE(kLogTag, "Error occurred during sending: errno %d", errno);
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void EspTcpPort::ReceiveTask(void *arg)
{
    EspTcpPort *port = (EspTcpPort *)arg;

    while (port->connected_)
    {
        int bytes = recv(port->socket_, port->receive_buffer_.data(), port->receive_buffer_.size(), 0);

        if (bytes < 0)
        {
            ESP_LOGE(kLogTag, "Error occurred during receiving: errno %d", errno);
        }
        else
        {
            bool buffered = false;

            while (!buffered)
            {
                if (!port->LockReceiver())
                {
                }
                else if ((port->receiver_ring_buffer_.Capacity() - port->receiver_ring_buffer_.Size()) < bytes)
                {
                    port->UnlockReceiver();
                }
                else
                {
                    port->receiver_ring_buffer_.Write(port->receive_buffer_.data(), bytes);
                    buffered = true;

                    port->UnlockReceiver();
                }
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }

    vTaskDelete(NULL);
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