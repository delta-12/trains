#include "tcp_server.h"

#include <iostream>
#include <memory>

#include "logger.h"

namespace controller_network
{

static const std::string kTcpServerLogTag("TCP SERVER");
static const std::string kTcpPortLogTag("TCP PORT");

TcpPort::TcpPort(asio::io_context &io_context) : socket_(io_context)
{
    receive_buffer_.reserve(kTcpPortBufferSize);

    LOGGER_LOG_DEBUG(std::cout, kTcpPortLogTag, "New port");
}

asio::ip::tcp::socket &TcpPort::GetSocket(void)
{
    return socket_;
}

void TcpPort::Start(void)
{
    connected_ = true;

    AsyncReceive();

    LOGGER_LOG_DEBUG(std::cout, kTcpPortLogTag, "Port started");
}

size_t TcpPort::Send(const uint8_t *const buffer, const size_t size)
{
    asio::async_write(socket_,
                      asio::buffer(buffer, size),
                      std::bind(&TcpPort::HandleWrite,
                                shared_from_this(),
                                asio::placeholders::error,
                                asio::placeholders::bytes_transferred));

    return size;     // All data is always sent
}

size_t TcpPort::SendAvailable(void)
{
    return SIZE_MAX;     // TcpPort does not buffer data to send
}

size_t TcpPort::Receive(uint8_t *const buffer, const size_t size)
{
    return receiver_ring_buffer_.Read(buffer, size);
}

size_t TcpPort::ReceiveAvailable(void)
{
    return receiver_ring_buffer_.Size();
}

bool TcpPort::Connected(void)
{
    return connected_;
}

void TcpPort::AsyncReceive(void)
{
    asio::async_read(socket_,
                     asio::buffer(receive_buffer_),
                     std::bind(&TcpPort::HandleRead,
                               shared_from_this(),
                               asio::placeholders::error,
                               asio::placeholders::bytes_transferred));
}

void TcpPort::HandleWrite(const std::error_code &error, const size_t bytes_transferred) const
{
    if (error)
    {
        LOGGER_LOG_ERROR(std::cout, kTcpPortLogTag, "Error writing data");
    }
    else
    {
        LOGGER_LOG_VERBOSE(std::cout, kTcpPortLogTag, "{} bytes successfully written", bytes_transferred);
    }
}

void TcpPort::HandleRead(const std::error_code &error, const size_t bytes_transferred)
{
    if (error)
    {
        LOGGER_LOG_ERROR(std::cout, kTcpPortLogTag, "Error receiving data");
    }
    else if ((receiver_ring_buffer_.Capacity() - receiver_ring_buffer_.Size()) < bytes_transferred)
    {
        LOGGER_LOG_WARNING(std::cout, kTcpPortLogTag, "Not enough space to receive data");
    }
    else
    {
        for (const uint8_t &byte : receive_buffer_)
        {
            receiver_ring_buffer_.Write(&byte, 1);
        }
        receive_buffer_.clear();

        LOGGER_LOG_VERBOSE(std::cout, kTcpPortLogTag, "{} bytes successfully received", bytes_transferred);
    }

    AsyncReceive();
}

TcpServer::TcpServer(const uint16_t port) : acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
    StartAccept();

    LOGGER_LOG_DEBUG(std::cout, kTcpServerLogTag, "Server started");
}

TcpServer::~TcpServer(void)
{
    io_context_.stop();

    LOGGER_LOG_DEBUG(std::cout, kTcpServerLogTag, "Server stopped");
}

void TcpServer::RunFor(const std::chrono::milliseconds duration)
{
    io_context_.run_for(duration);
}

void TcpServer::StartAccept(void)
{
    std::shared_ptr<TcpPort> port = std::make_shared<TcpPort>(io_context_);

    acceptor_.async_accept(port->GetSocket(),
                           std::bind(&TcpServer::HandleAccept,
                                     this,
                                     port,
                                     asio::placeholders::error));
}

void TcpServer::HandleAccept(std::shared_ptr<TcpPort> port, const std::error_code &error)
{
    if (!error)
    {
        port->Start();

        // TODO remove test
        uint8_t test[] = "foobar";
        port->Send(test, sizeof(test));

        // TODO pass port to callback function to pass to controller handler

        LOGGER_LOG_DEBUG(std::cout, kTcpServerLogTag, "New connection accepted");
    }
    else
    {
        LOGGER_LOG_ERROR(std::cout, kTcpServerLogTag, "Error accepting new connection");
    }

    StartAccept();
}

} // namespace controller_network