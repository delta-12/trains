/*****************************************************************************
* @file tcp_server.h
*
* @brief Implements a simple TCP server for commuicating with controllers.
*****************************************************************************/
#ifndef TRAINS_SRC_CONTROLLER_NETWORK_INC_TCP_SERVER_H
#define TRAINS_SRC_CONTROLLER_NETWORK_INC_TCP_SERVER_H

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "types.h"
#include "ring_buffer.h"

#include "asio.hpp"

namespace controller_network
{

const size_t kTcpPortBufferSize = 2048;

// TODO handle socket close
class TcpPort : public std::enable_shared_from_this<TcpPort>, public types::Port
{
    public:
        TcpPort(asio::io_context &io_context);
        asio::ip::tcp::socket &GetSocket(void);
        void Start(void);
        size_t Send(const uint8_t *const buffer, const size_t size);
        size_t SendAvailable(void);
        size_t Receive(uint8_t *const buffer, const size_t size);
        size_t ReceiveAvailable(void);
        bool Connected(void);

    private:
        static size_t CheckReadComplete(const asio::error_code& error, std::size_t bytes_transferred);
        void AsyncReceive(void);
        void HandleWrite(const std::error_code &error, const size_t bytes_transferred) const;
        void HandleRead(const std::error_code &error, const size_t bytes_transferred);

        asio::ip::tcp::socket socket_;
        bool connected_ = false;
        std::array<uint8_t, kTcpPortBufferSize> receive_buffer_;
        RingBuffer<uint8_t, kTcpPortBufferSize> receiver_ring_buffer_;
};

class TcpServer
{
    public:
        TcpServer(const uint16_t port, std::function<void(std::shared_ptr<types::Port> port)> on_accept);
        ~TcpServer(void);
        void RunFor(const std::chrono::milliseconds duration);

    private:
        void StartAccept(void);
        void HandleAccept(std::shared_ptr<TcpPort> port, const std::error_code &errord);

        asio::io_context io_context_;
        asio::ip::tcp::acceptor acceptor_;
        std::function<void(std::shared_ptr<types::Port> port)> on_accept_;
};

} // namespace controller_network

#endif // TRAINS_SRC_CONTROLLER_NETWORK_INC_TCP_SERVER_H