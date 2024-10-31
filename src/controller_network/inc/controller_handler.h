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

typedef enum
{
    CONTROLLERTYPE_WAYSIDE,
    CONTROLLERTYPE_TRAIN,
    CONTROLLERTYPE_MAX
} ControllerType;

types::Error LookupWaysideController(types::WaysideId &wayside, const types::TrackId track, const types::BlockId block);

template <size_t buffer_size>
class ControllerHandler
{
    public:
        void AddPort(std::unique_ptr<controller_network::ControllerPort> port);
        bool IsControllerConnected(const ControllerType type, const types::ControllerId controller) const;
        types::Error Update(ctc::Ctc &ctc_office);
        types::Error Update(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator);

    private:
        void MapConnections(void);
        void RemoveDisconnectedPorts(void);
        types::Error ReceiveMessages(ctc::Ctc &ctc_office);
        types::Error ReceiveMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator);
        // types::Error ReceiveMessageFromPort(ctc::Ctc &ctc_office, std::unique_ptr<controller_network::ControllerPort> port);
        // types::Error ReceiveMessageFromPort(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator, std::unique_ptr<controller_network::ControllerPort> port);
        types::Error SendMessages(ctc::Ctc &ctc_office);
        types::Error SendMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator);
        types::Error HandleTrackCircuitData(const size_t message_size, simulator::Simulator &world_simulator);
        types::Error SendTrackCircuitData(const types::TrackCircuitData &data);

        std::vector<std::unique_ptr<controller_network::ControllerPort>> unmapped_ports_;
        std::array<std::unordered_map<types::ControllerId, std::unique_ptr<controller_network::ControllerPort>>, CONTROLLERTYPE_MAX> connected_controllers_;
        std::array<uint8_t, buffer_size> message_buffer_;
};

template <size_t buffer_size>
void ControllerHandler<buffer_size>::AddPort(std::unique_ptr<controller_network::ControllerPort> port)
{
    unmapped_ports_.emplace_back(std::move(port));
}

template <size_t buffer_size>
bool ControllerHandler<buffer_size>::IsControllerConnected(const ControllerType type, const types::ControllerId controller) const
{
    bool connected = false;

    if (type < CONTROLLERTYPE_MAX)
    {
        connected = connected_controllers_[type].contains(controller);
    }

    return connected;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::Update(ctc::Ctc &ctc_office)
{
    types::Error error = ReceiveMessages(ctc_office);

    if (types::ERROR_NONE == error)
    {
        error = SendMessages(ctc_office);
    }

    MapConnections();
    RemoveDisconnectedPorts();

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

    MapConnections();
    RemoveDisconnectedPorts();

    return error;
}

template <size_t buffer_size>
void ControllerHandler<buffer_size>::MapConnections(void)
{
    unmapped_ports_.erase(std::remove_if(unmapped_ports_.begin(), unmapped_ports_.end(), [this](const std::unique_ptr<controller_network::ControllerPort> &port)
        {
            bool controller_connected = false;
            MessageType message_type  = MESSAGETYPE_RESERVED;
            size_t message_size       = port->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());
            // controller_messages::Connection connection_message;

            if ((message_size == 0) || (MESSAGETYPE_CONNECTION != message_type))
            {
                // Not a connection message, do nothing
            }
            // else if (!connection_message.ParseFromArray(message_buffer_.data(), message_size))
            // {
            //     // Invalid message, do nothing
            // }
            // else if (static_cast<ControllerType>(connection_message.controller_type()) >= CONTROLLERTYPE_MAX)
            // {
            //     // Invalid controller type, do nothing
            // }
            // else if (!connected_controllers_[connection_message.controller_type()].contains(connection_message.controller_id()))
            // {
            //     // TODO send success/error message back to controller
            //     connected_controllers_[connection_message.controller_type()].emplace(connection_message.controller_id(), std::move(port));

            //     controller_connected = true;
            // }

            return controller_connected;
        }), unmapped_ports_.end());
}

template <size_t buffer_size>
void ControllerHandler<buffer_size>::RemoveDisconnectedPorts(void)
{
    unmapped_ports_.erase(std::remove_if(unmapped_ports_.begin(), unmapped_ports_.end(), [](const std::unique_ptr<controller_network::ControllerPort> &port)
        {
            return !port->Connected();
        }), unmapped_ports_.end());

    

    for (std::unordered_map<types::ControllerId, std::unique_ptr<controller_network::ControllerPort>> &connected_controllers_map : connected_controllers_)
    {
        std::unordered_map<types::ControllerId, std::unique_ptr<controller_network::ControllerPort>>::iterator i = connected_controllers_map.begin();
        while (i != connected_controllers_map.end())
        {
            if (!i->second->Connected())
            {
                i = connected_controllers_map.erase(i);
            }
            else
            {
                ++i;
            }
        }
    }
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

    for (size_t i = 0; i < connected_controllers_.size(); i++)
    {
        for (const std::pair<types::ControllerId, std::unique_ptr<controller_network::ControllerPort> &> port : connected_controllers_[i])
        {
            // TODO error handling
            // error = ReceiveMessageFromPort(ctc_office, world_simulator, port.second);
        }
    }

    return error;
}

// template <size_t buffer_size>
// types::Error ControllerHandler<buffer_size>::ReceiveMessageFromPort(ctc::Ctc &ctc_office, std::unique_ptr<controller_network::ControllerPort> port)
// {
//     return types::ERROR_INVALID_FORMAT;
// }

// template <size_t buffer_size>
// types::Error ControllerHandler<buffer_size>::ReceiveMessageFromPort(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator, std::unique_ptr<controller_network::ControllerPort> port)
// {
//     types::Error error        = types::ERROR_NONE;
//     MessageType  message_type = MESSAGETYPE_RESERVED;
//     size_t       message_size = port->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());

//     if ((message_size > 0) && (MESSAGETYPE_RESERVED != message_type))
//     {
//         switch (message_type)
//         {
//         case MESSAGETYPE_TRACK_CIRCUIT_DATA:
//             error = HandleTrackCircuitData(message_size, world_simulator);
//             break;
//         default:
//             break;
//         }
//     }

//     return error;
// }


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

    std::vector<types::TrackCircuitData> track_circuit_data = ctc_office.GetSuggestedSpeedsAndAuthorities();
    for (const types::TrackCircuitData &data : track_circuit_data)
    {
        // TODO error handle
        error = SendTrackCircuitData(data);
    }

    // TODO

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::HandleTrackCircuitData(const size_t message_size, simulator::Simulator &world_simulator)
{
    types::Error error = types::ERROR_NONE;
    // controller_messages::TrackCircuitData track_circuit_data_message;

    // if (!track_circuit_data_message.ParseFromArray(message_buffer_.data(), message_size))
    // {
    //     error = types::ERROR_INVALID_FORMAT;
    // }
    // else
    // {
    //     types::TrackCircuitData track_circuit_data(track_circuit_data_message.track, track_circuit_data_message.block,
    //                                                track_circuit_data_message.speed_meters_per_second, track_circuit_data_message.authority);

    //     error = world_simulator.SetTrackCircuitData(track_circuit_data);
    // }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::SendTrackCircuitData(const types::TrackCircuitData &data)
{
    types::WaysideId wayside;
    types::Error     error = LookupWaysideController(wayside, data.track, data.block);

    // controller_messages::TrackCircuitData message;
    // message.set_track(static_cast<controller_messages::TrackId>(data.track));
    // message.set_block(data.block);
    // message.set_speed_meters_per_second(data.speed);
    // message.set_authority(data.authority);
    // size_t message_size = message.ByteSizeLong();

    // if (types::ERROR_NONE != error)
    // {
    //     // Failed to lookup wayside controller, do nothing
    // }
    // else if (!connected_controllers_[CONTROLLERTYPE_WAYSIDE].contains(wayside))
    // {
    //     // Wayside controller is not connected, do nothing
    // }
    // else if (!message.SerializeToArray(message_buffer_, message_buffer_.size()))
    // {
    //     // Failed to serialize message, do nothing
    // }
    // else
    // {
    //     // TODO error handle
    //     connected_controllers_[CONTROLLERTYPE_WAYSIDE][wayside]->SendMessage(MESSAGETYPE_TRACK_CIRCUIT_DATA, message_buffer_, message_size);
    // }

    return error;
}

} // namespace controller_network

#endif // TRAINS_SRC_COMMON_INC_CONTROLLER_HANDLER_H