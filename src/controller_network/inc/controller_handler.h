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

#include "block_outputs.pb.h"
#include "block_states.pb.h"
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
        void AddPort(std::unique_ptr<ControllerPort> port);
        bool IsControllerConnected(const ControllerType type, const types::ControllerId controller) const;
        types::Error Update(ctc::Ctc &ctc_office);
        types::Error Update(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator);

    private:
        void MapConnections(void);
        void RemoveDisconnectedPorts(void);
        types::Error ReceiveMessages(ctc::Ctc &ctc_office);
        types::Error ReceiveMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator);
        types::Error ReceiveMessagesFromControllers(ctc::Ctc &ctc_office, std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>> &controllers);
        types::Error ReceiveMessagesFromControllers(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator, std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>> &controllers);
        types::Error ReceiveMessageFromPort(ctc::Ctc &ctc_office, const std::unique_ptr<ControllerPort> &port);
        types::Error ReceiveMessageFromPort(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator, const std::unique_ptr<ControllerPort> &port);
        types::Error SendMessages(ctc::Ctc &ctc_office);
        types::Error SendMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator);
        types::Error HandleBlockStates(const size_t message_size, ctc::Ctc &ctc_office);
        types::Error HandleTrackCircuitData(const size_t message_size, simulator::Simulator &world_simulator);
        types::Error HandleBlockOutputs(const size_t message_size, simulator::Simulator &world_simulator);
        types::Error SendTrackCircuitData(const ctc::Ctc &ctc_office);

        std::vector<std::unique_ptr<ControllerPort>> unmapped_ports_;
        std::array<std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>>, CONTROLLERTYPE_MAX> connected_controllers_;
        std::array<uint8_t, buffer_size> message_buffer_;
};

template <size_t buffer_size>
void ControllerHandler<buffer_size>::AddPort(std::unique_ptr<ControllerPort> port)
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

    if (types::Error::ERROR_NONE == error)
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

    if (types::Error::ERROR_NONE == error)
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
    unmapped_ports_.erase(std::remove_if(unmapped_ports_.begin(), unmapped_ports_.end(), [this](std::unique_ptr<ControllerPort> &port)
        {
            bool controller_connected = false;
            MessageType message_type  = MESSAGETYPE_NONE;
            size_t message_size       = port->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());
            controller_messages::Connection connection_message;

            if (MESSAGETYPE_CONNECTION != message_type)
            {
                // Not a connection message, do nothing
            }
            else if (!connection_message.ParseFromArray(message_buffer_.data(), message_size))
            {
                // Invalid message, do nothing
            }
            else if (static_cast<ControllerType>(connection_message.controller_type()) >= CONTROLLERTYPE_MAX)
            {
                // Invalid controller type, do nothing
            }
            else if (!connected_controllers_[connection_message.controller_type()].contains(connection_message.controller_id()))
            {
                // TODO NNF-228 send success/error message back to controller
                connected_controllers_[connection_message.controller_type()].emplace(connection_message.controller_id(), std::move(port));

                controller_connected = true;
            }

            return controller_connected;
        }),
                          unmapped_ports_.end());
}

template <size_t buffer_size>
void ControllerHandler<buffer_size>::RemoveDisconnectedPorts(void)
{
    unmapped_ports_.erase(std::remove_if(unmapped_ports_.begin(), unmapped_ports_.end(), [](const std::unique_ptr<ControllerPort> &port)
        {
            return !port->Connected();
        }),
                          unmapped_ports_.end());

    for (std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>> &connected_controllers_map : connected_controllers_)
    {
        std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>>::iterator i = connected_controllers_map.begin();
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
    types::Error error = types::Error::ERROR_NONE;

    for (std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>> &connected_controllers_map : connected_controllers_)
    {
        error = ReceiveMessagesFromControllers(ctc_office, connected_controllers_map);

        if (types::Error::ERROR_NONE != error)
        {
            break;
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::ReceiveMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator)
{
    types::Error error = types::Error::ERROR_NONE;

    for (std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>> &connected_controllers_map : connected_controllers_)
    {
        error = ReceiveMessagesFromControllers(ctc_office, world_simulator, connected_controllers_map);

        if (types::Error::ERROR_NONE != error)
        {
            break;
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::ReceiveMessagesFromControllers(ctc::Ctc &ctc_office, std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>> &controllers)
{
    types::Error error = types::Error::ERROR_NONE;

    for (std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>>::iterator i = controllers.begin(); i != controllers.end(); ++i)
    {
        error = ReceiveMessageFromPort(ctc_office, i->second);

        if (types::Error::ERROR_NONE != error)
        {
            break;
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::ReceiveMessagesFromControllers(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator, std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>> &controllers)
{
    types::Error error = types::Error::ERROR_NONE;

    for (std::unordered_map<types::ControllerId, std::unique_ptr<ControllerPort>>::iterator i = controllers.begin(); i != controllers.end(); ++i)
    {
        error = ReceiveMessageFromPort(ctc_office, world_simulator, i->second);

        if (types::Error::ERROR_NONE != error)
        {
            break;
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::ReceiveMessageFromPort(ctc::Ctc &ctc_office, const std::unique_ptr<ControllerPort> &port)
{
    types::Error error        = types::Error::ERROR_NONE;
    MessageType  message_type = MESSAGETYPE_NONE;
    size_t       message_size = port->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());

    if ((message_size > 0) && (MESSAGETYPE_NONE != message_type))
    {
        switch (message_type)
        {
        case MESSAGETYPE_BLOCK_STATES:
            error = HandleBlockStates(message_size, ctc_office);
            break;
        default:
            break;
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::ReceiveMessageFromPort(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator, const std::unique_ptr<ControllerPort> &port)
{
    types::Error error        = ReceiveMessageFromPort(ctc_office, port);
    MessageType  message_type = MESSAGETYPE_NONE;
    size_t       message_size = port->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());

    if ((message_size > 0) && (MESSAGETYPE_NONE != message_type))
    {
        switch (message_type)
        {
        case MESSAGETYPE_TRACK_CIRCUIT_DATA:
            error = HandleTrackCircuitData(message_size, world_simulator);
            break;
        case MESSAGETYPE_BLOCK_OUTPUTS:
            error = HandleBlockOutputs(message_size, world_simulator);
            break;
        // TODO NNF-229 receive train command from train controller
        // TODO NNF-229 receive track information from train controller
        default:
            break;
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::SendMessages(ctc::Ctc &ctc_office)
{
    types::Error error = SendTrackCircuitData(ctc_office);

    if (types::Error::ERROR_NONE == error)
    {
        // TODO NNF-230 send maintenance mode to wayside controller
    }

    if (types::Error::ERROR_NONE == error)
    {
        // TODO NNF-230 send suggested switch states to wayside controller
    }

    // TODO NNF-229 send train dispatch to train controller

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::SendMessages(ctc::Ctc &ctc_office, simulator::Simulator &world_simulator)
{
    types::Error error = SendMessages(ctc_office);

    // TODO NNF-230 send block occupancies to wayside controller
    // TODO NNF-229 send train feedback to train controller
    // TODO NNF-229 send train failures to train controller
    // TODO NNF-229 send beacon data to train controller
    // TODO NNF-229 send track circuit data to train controller
    (void)(world_simulator); // UNUSED

    std::vector<types::TrackCircuitData> track_circuit_data_vector = ctc_office.GetSuggestedSpeedsAndAuthorities();

    for(auto &[train_id,controller_port] : connected_controllers_[CONTROLLERTYPE_TRAIN])
    {
        types::TrackCircuitData track_circuit_data;
        world_simulator.GetTrackCircuitData(train_id,track_circuit_data);

        controller_messages::TrackCircuitData message;


        message.set_track(static_cast<controller_messages::TrackId>(track_circuit_data.track));
        message.set_block(track_circuit_data.block);
        message.set_speed_meters_per_second(track_circuit_data.speed);
        message.set_authority(track_circuit_data.authority);
        message.set_polarity(track_circuit_data.polarity);
        size_t message_size = message.ByteSizeLong();


        message.SerilizeToArray(message_buffer_.data(),message_buffer_.size());
        connected_controllers_[CONTROLLERTYPE_TRAIN][train_id]->SendMessage(MESSAGETYPE_TRACK_CIRCUIT_DATA, message_buffer_.data(),message_size);
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::HandleBlockStates(const size_t message_size, ctc::Ctc &ctc_office)
{
    types::Error                     error = types::Error::ERROR_NONE;
    controller_messages::BlockStates block_states_message;

    if (!block_states_message.ParseFromArray(message_buffer_.data(), message_size))
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    else
    {
        size_t                         states = block_states_message.states_size();
        std::vector<types::BlockState> block_states;
        block_states.reserve(states);

        for (size_t i = 0; i < states; i++)
        {
            const controller_messages::BlockState &block_state = block_states_message.states(i);

            block_states.emplace_back(block_state.block(), block_state.occupied(), block_state.track_failure());
        }

        error = ctc_office.SetBlockStates(static_cast<types::TrackId>(block_states_message.track()), block_states);
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::HandleTrackCircuitData(const size_t message_size, simulator::Simulator &world_simulator)
{
    types::Error                          error = types::Error::ERROR_NONE;
    controller_messages::TrackCircuitData track_circuit_data_message;

    if (!track_circuit_data_message.ParseFromArray(message_buffer_.data(), message_size))
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    else
    {
        types::TrackCircuitData track_circuit_data(static_cast<types::TrackId>(track_circuit_data_message.track()),
                                                   track_circuit_data_message.block(),
                                                   track_circuit_data_message.speed_meters_per_second(),
                                                   track_circuit_data_message.authority());

        error = world_simulator.SetTrackCircuitData(track_circuit_data);
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::HandleBlockOutputs(const size_t message_size, simulator::Simulator &world_simulator)
{
    types::Error                      error = types::Error::ERROR_NONE;
    controller_messages::BlockOutputs block_outputs_message;

    if (!block_outputs_message.ParseFromArray(message_buffer_.data(), message_size))
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    else
    {
        types::TrackId track = static_cast<types::TrackId>(block_outputs_message.track());
        types::BlockId block = block_outputs_message.block();

        if (block_outputs_message.has_switched())
        {
            error = world_simulator.SetSwitchState(track, block, block_outputs_message.switched());
        }
        if ((block_outputs_message.has_crossing()) && (types::Error::ERROR_NONE == error))
        {
            error = world_simulator.SetCrossingState(track, block, block_outputs_message.crossing());
        }
        if ((block_outputs_message.has_traffic_light_color()) && (types::Error::ERROR_NONE == error))
        {
            error = world_simulator.SetTrafficLight(track, block, static_cast<types::TrafficLightColor>(block_outputs_message.traffic_light_color()));
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error ControllerHandler<buffer_size>::SendTrackCircuitData(const ctc::Ctc &ctc_office)
{
    types::Error                         error              = types::Error::ERROR_NONE;
    std::vector<types::TrackCircuitData> track_circuit_data = ctc_office.GetSuggestedSpeedsAndAuthorities();

    for (const types::TrackCircuitData &data : track_circuit_data)
    {
        types::WaysideId wayside;
        error = LookupWaysideController(wayside, data.track, data.block);

        controller_messages::TrackCircuitData message;
        message.set_track(static_cast<controller_messages::TrackId>(data.track));
        message.set_block(data.block);
        message.set_speed_meters_per_second(data.speed);
        message.set_authority(data.authority);
        size_t message_size = message.ByteSizeLong();

        if (types::Error::ERROR_NONE != error)
        {
            // Failed to lookup wayside controller, do nothing
        }
        else if (!connected_controllers_[CONTROLLERTYPE_WAYSIDE].contains(wayside))
        {
            error = types::Error::ERROR_INVALID_CONTROLLER;
        }
        else if (!message.SerializeToArray(message_buffer_.data(), message_buffer_.size()))
        {
            error = types::Error::ERROR_INVALID_FORMAT;
        }
        else if (message_size != connected_controllers_[CONTROLLERTYPE_WAYSIDE][wayside]->SendMessage(MESSAGETYPE_TRACK_CIRCUIT_DATA, message_buffer_.data(), message_size))
        {
            error = types::Error::ERROR_INVALID_SIZE;
        }

        if (types::Error::ERROR_NONE != error)
        {
            break;
        }
    }

    return error;
}

} // namespace controller_network

#endif // TRAINS_SRC_COMMON_INC_CONTROLLER_HANDLER_H