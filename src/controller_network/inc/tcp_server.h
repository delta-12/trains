/*****************************************************************************
* @file tcp_server.h
*
* @brief Implements a simple TCP server for commuicating with controllers.
*****************************************************************************/
#ifndef TRAINS_SRC_CONTROLLER_NETWORK_INC_TCP_SERVER_H
#define TRAINS_SRC_CONTROLLER_NETWORK_INC_TCP_SERVER_H

#include <chrono>
#include <memory>
#include <vector>

#include "asio.hpp"

#include "types.h"
#include "ring_buffer.h"

namespace controller_network
{

const size_t kTcpPortBufferSize = 2048;

class TcpPort : public types::Port, std::enable_shared_from_this<TcpPort>
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
        void AsyncReceive(void);
        void HandleWrite(const std::error_code &error, const size_t bytes_transferred) const;
        void HandleRead(const std::error_code &error, const size_t bytes_transferred);

        asio::ip::tcp::socket socket_;
        bool connected_ = false;
        std::vector<uint8_t> receive_buffer_;
        RingBuffer<uint8_t, kTcpPortBufferSize> receiver_ring_buffer_;
};

class TcpServer
{
    public:
        TcpServer(const uint16_t port);
        ~TcpServer(void);
        void RunFor(const std::chrono::milliseconds duration);
    // TODO add method to register callback that attaches new connection to controller handler

    private:
        void StartAccept(void);
        void HandleAccept(std::shared_ptr<TcpPort> port, const std::error_code &errord);

        asio::io_context io_context_;
        asio::ip::tcp::acceptor acceptor_;
};

} // namespace controller_network

#endif // TRAINS_SRC_CONTROLLER_NETWORK_INC_TCP_SERVER_H