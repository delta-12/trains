/*****************************************************************************
* @file wayside_controller_handler.h
*
* @brief Implements a handler for the wayside controller.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_HANDLER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_HANDLER_H

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#include "block_occupancies.pb.h"
#include "block_states.pb.h"
#include "controller_port.h"
#include "lookup_table.h"
#include "track_circuit_data.pb.h"
#include "types.h"
#include "wayside_controller.h"

namespace wayside_controller
{

template <size_t buffer_size>
class SoftwareWaysideControllerHandler
{
    public:
        SoftwareWaysideControllerHandler(std::unique_ptr<controller_network::ControllerPort> controller_port,
                                         const types::TrackId track,
                                         const std::vector<WaysideBlock> &blocks);
        types::Error Update(void);

    private:
        Error GetInput(const InputId input, IoSignal &signal) const;
        Error SetInput(const InputId input, const IoSignal signal);
        Error SetBlockOccupancy(const types::BlockId block, const bool occupied);
        types::Error ReceiveMessages(void);
        types::Error HandleTrackCircuitData(const size_t message_size);
        types::Error HandleBlockOccupancies(const size_t message_size);
        types::Error SendBlockStates(const std::vector<types::BlockState> &block_states) const;

        std::function<Error(const InputId input, IoSignal &signal)> get_input_ = [this](const InputId input, IoSignal &signal){
                                                                                     return GetInput(input, signal);
                                                                                 };
        std::unique_ptr<controller_network::ControllerPort> controller_port_;
        std::array<uint8_t, buffer_size> message_buffer_;
        std::array<IoSignal, kTotalInputs> inputs_ = {IoSignal::IOSIGNAL_LOW};
        std::unordered_map<types::BlockId, InputId> inputs_lookup_;
        WaysideController wayside_controller_;
        types::TrackId track_;
};

template <size_t buffer_size>
SoftwareWaysideControllerHandler<buffer_size>::SoftwareWaysideControllerHandler(std::unique_ptr<controller_network::ControllerPort> controller_port,
                                                                                const types::TrackId track,
                                                                                const std::vector<WaysideBlock> &blocks)
    : controller_port_(std::move(controller_port)), wayside_controller_(get_input_, blocks), track_(track)
{
    for (const wayside_controller::WaysideBlock &block : blocks)
    {
        inputs_lookup_[block.block] = block.track_circuit_input;
    }
}

template <size_t buffer_size>
types::Error SoftwareWaysideControllerHandler<buffer_size>::Update(void)
{
    types::Error                   error = ReceiveMessages();
    std::vector<types::BlockState> block_states;

    if (types::Error::ERROR_NONE != error)
    {
        // Do nothing
    }
    else if (Error::ERROR_NONE != wayside_controller_.GetBlockStates(block_states))
    {
        error = types::Error::ERROR_INVALID_BLOCK;
    }
    else
    {
        error = SendBlockStates(block_states);
    }

    return error;
}

template <size_t buffer_size>
Error SoftwareWaysideControllerHandler<buffer_size>::GetInput(const InputId input, IoSignal &signal) const
{
    Error error = Error::ERROR_INVALID_INPUT;

    if (input < inputs_.size())
    {
        signal = inputs_[input];

        error = Error::ERROR_NONE;
    }

    return error;
}

template <size_t buffer_size>
Error SoftwareWaysideControllerHandler<buffer_size>::SetInput(const InputId input, const IoSignal signal)
{
    Error error = Error::ERROR_INVALID_INPUT;

    if (input < inputs_.size())
    {
        inputs_[input] = signal;

        error = Error::ERROR_NONE;
    }

    return error;
}

template <size_t buffer_size>
Error SoftwareWaysideControllerHandler<buffer_size>::SetBlockOccupancy(const types::BlockId block, const bool occupied)
{
    Error    error  = Error::ERROR_INVALID_BLOCK;
    IoSignal signal = IoSignal::IOSIGNAL_HIGH;

    if (!inputs_lookup_.contains(block))
    {
        // Do nothing
    }
    else
    {
        error = SetInput(inputs_lookup_[block], signal);
    }

    return error;
}

template <size_t buffer_size>
types::Error SoftwareWaysideControllerHandler<buffer_size>::ReceiveMessages(void)
{
    types::Error                    error        = types::Error::ERROR_NONE;
    controller_network::MessageType message_type = controller_network::MESSAGETYPE_NONE;
    size_t                          message_size = controller_port_->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());

    while ((message_size > 0) && (controller_network::MESSAGETYPE_NONE != message_type))
    {
        switch (message_type)
        {
        case controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA:
            error = HandleTrackCircuitData(message_size);
            break;
        case controller_network::MESSAGETYPE_BLOCK_OCCUPANCIES:
            error = HandleBlockOccupancies(message_size);
            break;
        default:
            break;
        }

        message_size = controller_port_->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());
    }

    return error;
}

template <size_t buffer_size>
types::Error SoftwareWaysideControllerHandler<buffer_size>::HandleTrackCircuitData(const size_t message_size)
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

        wayside_controller_.GetCommandedSpeedAndAuthority(track_circuit_data);
        track_circuit_data_message.set_speed_meters_per_second(track_circuit_data.speed);
        track_circuit_data_message.set_authority(track_circuit_data.authority);
        size_t message_size = track_circuit_data_message.ByteSizeLong();

        if (!track_circuit_data_message.SerializeToArray(message_buffer_.data(), message_buffer_.size()))
        {
            error = types::Error::ERROR_INVALID_FORMAT;
        }
        else if (message_size != controller_port_->SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, message_buffer_.data(), message_size))
        {
            error = types::Error::ERROR_INVALID_SIZE;
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error SoftwareWaysideControllerHandler<buffer_size>::HandleBlockOccupancies(const size_t message_size)
{
    types::Error                          error = types::Error::ERROR_NONE;
    controller_messages::BlockOccupancies block_occupancies_message;

    if (!block_occupancies_message.ParseFromArray(message_buffer_.data(), message_size))
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    else
    {
        for (size_t i = 0; i < block_occupancies_message.occupancies_size(); i++)
        {
            const controller_messages::BlockOccupany &block_occupancy = block_occupancies_message.occupancies(i);

            if (Error::ERROR_NONE != SetBlockOccupancy(block_occupancy.block(), block_occupancy.occupied()))
            {
                error = types::Error::ERROR_INVALID_BLOCK;
                break;
            }
        }
    }

    return error;
}

template <size_t buffer_size>
types::Error SoftwareWaysideControllerHandler<buffer_size>::SendBlockStates(const std::vector<types::BlockState> &block_states) const
{
    types::Error                     error = types::Error::ERROR_NONE;
    controller_messages::BlockStates block_states_message;

    block_states_message.set_track(static_cast<controller_messages::TrackId>(track_));

    for (const types::BlockState &block_state : block_states)
    {
        controller_messages::BlockState* block_state_message = block_states_message.add_states();
        block_state_message->set_block(block_state.block);
        block_state_message->set_occupied(block_state.occupied);
        block_state_message->set_track_failure(block_state.track_failure);
    }

    size_t message_size = block_states_message.ByteSizeLong();

    if (!block_states_message.SerializeToArray(message_buffer_.data(), message_buffer_.size()))
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    else if (message_size != controller_port_->SendMessage(controller_network::MESSAGETYPE_BLOCK_STATES, message_buffer_.data(), message_size))
    {
        error = types::Error::ERROR_INVALID_SIZE;
    }

    return error;
}

} // namespace wayside_controller

#endif // namespace TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_HANDLER_H