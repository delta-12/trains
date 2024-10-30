/*****************************************************************************
* @file controller_handler.h
*
* @brief Sends and receives messages to and from controllers.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_CONTROLLER_HANDLER_H
#define TRAINS_SRC_COMMON_INC_CONTROLLER_HANDLER_H

#include <array>
#include <memory>
#include <unordered_map>

#include "connection.pb.h"
#include "controller_network_protocols.h"
#include "controller_port.h"
#include "ctc.h"
#include "simulator.h"
#include "track_circuit_data.pb.h"
#include "types.h"

namespace controller_network
{

template <size_t buffer_size>
class ControllerHandler
{
    public:
        void AddPort(std::unique_ptr<controller_network::ControllerPort> port);
        bool IsControllerConnected(const types::ControllerId controller) const;
        types::Error Update(ctc::Ctc &ctc_office);
        types::Error Update(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator);

    private:
        void MapConnections(void);
        void RemoveDisconnectedPorts(void);
        types::Error ReceiveMessages(ctc::Ctc &ctc_office);
        types::Error ReceiveMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator);
        types::Error SendMessages(ctc::Ctc &ctc_office);
        types::Error SendMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator);
        types::Error HandleTrackCircuitData(const size_t message_size, simulator::Simulator &world_simulator);

        std::vector<std::unique_ptr<controller_network::ControllerPort>> unmapped_ports_;
        std::unordered_map<types::ControllerId, std::unique_ptr<controller_network::ControllerPort>> connected_controllers_;
        std::array<uint8_t, buffer_size> message_buffer_;
};

template <size_t buffer_size>
void ControllerHandler<buffer_size>::AddPort(std::unique_ptr<controller_network::ControllerPort> port)
{
    unmapped_ports_.emplace_back(std::move(port));
}

template <size_t buffer_size>
bool ControllerHandler<buffer_size>::IsControllerConnected(const types::ControllerId controller) const
{
    return connected_controllers_.contains(controller);
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::Update(ctc::Ctc &ctc_office)
{
    types::Error error = ReceiveMessages(ctc_office);

    if (types::ERROR_NONE == error)
    {
        error = SendMessages(ctc_office);
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::Update(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator)
{
    types::Error error = ReceiveMessages(ctc_office, world_simulator);

    if (types::ERROR_NONE == error)
    {
        error = SendMessages(ctc_office, world_simulator);
    }

    return error;
}

template <size_t buffer_size>
void ControllerHandler<buffer_size>::MapConnections(void)
{
    std::remove_if(unmapped_ports_.begin(), unmapped_ports_.end(), [this](std::unique_ptr<controller_network::ControllerPort> &port)
        {
            bool controller_connected = false;
            MessageType message_type  = MESSAGETYPE_RESERVED;
            size_t message_size       = port->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());
            controller_messages::Connection connection_message;

            if ((message_size == 0) || (MESSAGETYPE_CONNECTION != message_type))
            {
                // Not a connection message, do nothing
            }
            else if (!connection_message.ParseFromArray(message_buffer_.data(), message_size))
            {
                // Invalid message, do nothing
            }
            else if (!connected_controllers_.contains(connection_message.controller_id()))
            {
                connected_controllers_.emplace(connection_message.controller_id(), std::move(port));

                controller_connected = true;
            }

            return controller_connected;
        });
}

template <size_t buffer_size>
void ControllerHandler<buffer_size>::RemoveDisconnectedPorts(void)
{
    std::remove_if(unmapped_ports_.begin(), unmapped_ports_.end(), [](const std::unique_ptr<controller_network::ControllerPort> &port)
        {
            return !port->Connected();
        });

    std::remove_if(connected_controllers_.begin(), connected_controllers_.end(), [](const std::pair<types::ControllerId, std::unique_ptr<controller_network::ControllerPort> &> port)
        {
            return !port.second->Connected();
        });
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::ReceiveMessages(ctc::Ctc &ctc_office)
{
    types::Error error = types::ERROR_NONE;

    // TODO

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::ReceiveMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator)
{
    types::Error error = types::ERROR_NONE;
    MessageType  message_type;
    size_t       message_size;

    for (const std::pair<types::ControllerId, std::unique_ptr<controller_network::ControllerPort> &> port : connected_controllers_)
    {
        message_type = MESSAGETYPE_RESERVED;
        message_size = port.second->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());

        if ((message_size > 0) && (MESSAGETYPE_RESERVED != message_type))
        {
            switch (message_type)
            {
            case MESSAGETYPE_TRACK_CIRCUIT_DATA:
                error = HandleTrackCircuitData(message_size, world_simulator);
                break;
            default:
                break;
            }
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::SendMessages(ctc::Ctc &ctc_office)
{
    types::Error error = types::ERROR_NONE;

    // TODO

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::SendMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator)
{
    types::Error error = types::ERROR_NONE;

    // TODO

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::HandleTrackCircuitData(const size_t message_size, simulator::Simulator &world_simulator)
{
    types::Error                          error = types::ERROR_NONE;
    controller_messages::TrackCircuitData track_circuit_data_message;

    if (!track_circuit_data_message.ParseFromArray(message_buffer_.data(), message_size))
    {
        error = types::ERROR_INVALID_FORMAT;
    }
    else
    {
        types::TrackCircuitData track_circuit_data(track_circuit_data_message.track, track_circuit_data_message.block,
                                                   track_circuit_data_message.speed_meters_per_second, track_circuit_data_message.authority);

        error = world_simulator.SetTrackCircuitData(track_circuit_data);
    }

    return error;
}

} // namespace controller_network

#endif // TRAINS_SRC_COMMON_INC_CONTROLLER_HANDLER_H