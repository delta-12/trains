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

#include "esp_log.h"
#include "pb_decode.h"
#include "pb_encode.h"

#include "block_occupancies.pb.h"
#include "block_states.pb.h"
#include "connection.pb.h"
#include "controller_port.h"
#include "lookup_table.h"
#include "track_circuit_data.pb.h"
#include "types.h"
#include "wayside_controller.h"

namespace wayside_controller
{

template <size_t buffer_size>
class HardwareWaysideControllerHandler
{
    public:
        HardwareWaysideControllerHandler(const types::WaysideId wayside_id,
                                         const types::TrackId track,
                                         const std::vector<WaysideBlock> &blocks,
                                         std::unique_ptr<controller_network::ControllerPort> controller_port);
        types::Error Update(void);
        types::Error Connect(void);

    private:
        static bool DecodeBlockOccupancies(pb_istream_t *stream, const pb_field_t *field, void **arg);
        static bool EncodeBlockStates(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);

        Error GetInput(const InputId input, IoSignal &signal) const;
        Error SetInput(const InputId input, const IoSignal signal);
        Error SetBlockOccupancy(const types::BlockId block, const bool occupied);
        types::Error ReceiveMessages(void);
        types::Error HandleTrackCircuitData(void);
        types::Error HandleBlockOccupancies(void);
        types::Error SendBlockStates(const std::vector<types::BlockState> &block_states);

        std::function<Error(const InputId input, IoSignal &signal)> get_input_ = [this](const InputId input, IoSignal &signal){
                                                                                     return GetInput(input, signal);
                                                                                 };
        std::array<uint8_t, buffer_size> message_buffer_;
        std::array<IoSignal, kTotalInputs> inputs_ = {IoSignal::IOSIGNAL_LOW};
        std::unordered_map<types::BlockId, InputId> inputs_lookup_;
        WaysideController wayside_controller_;
        types::WaysideId id_;
        types::TrackId track_;
        std::unique_ptr<controller_network::ControllerPort> controller_port_;
};

template <size_t buffer_size>
HardwareWaysideControllerHandler<buffer_size>::HardwareWaysideControllerHandler(const types::WaysideId wayside_id,
                                                                                const types::TrackId track,
                                                                                const std::vector<WaysideBlock> &blocks,
                                                                                std::unique_ptr<controller_network::ControllerPort> controller_port)
    : wayside_controller_(get_input_, blocks), id_(wayside_id), track_(track), controller_port_(std::move(controller_port))
{
    for (const wayside_controller::WaysideBlock &block : blocks)
    {
        inputs_lookup_[block.block] = block.track_circuit_input;
    }
}

template <size_t buffer_size>
types::Error HardwareWaysideControllerHandler<buffer_size>::Update(void)
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
types::Error HardwareWaysideControllerHandler<buffer_size>::Connect(void)
{
    types::Error                    error = types::Error::ERROR_NONE;
    pb_ostream_t stream = pb_ostream_from_buffer(message_buffer_.data(), message_buffer_.size());
    controller_messages_Connection connection_message = controller_messages_Connection_init_zero;
    connection_message.controller_id = id_;
    connection_message.controller_type = controller_messages_ControllerType_CONTROLLER_TYPE_WAYSIDE;

    if(!pb_encode(&stream, controller_messages_Connection_fields, &connection_message))
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    else if (stream.bytes_written != controller_port_->SendMessage(controller_network::MESSAGETYPE_CONNECTION, message_buffer_.data(), stream.bytes_written))
    {
        error = types::Error::ERROR_INVALID_SIZE;
    }

    return error;
}

template <size_t buffer_size>
bool HardwareWaysideControllerHandler<buffer_size>::DecodeBlockOccupancies(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    (void)(field); // Unused

    bool decoded = false;
    size_t bytes_left = stream->bytes_left;
    HardwareWaysideControllerHandler<buffer_size> *hardware_wayside_controller_handler = (HardwareWaysideControllerHandler *)(*arg);
    static uint8_t buffer[sizeof(controller_messages_BlockOccupancy)] = {0U};
    pb_istream_t istream = pb_istream_from_buffer(buffer, bytes_left);
    controller_messages_BlockOccupancy block_occupancy_message = controller_messages_BlockOccupancy_init_default;

    if (nullptr == hardware_wayside_controller_handler)
    {
        /* Do not dereference nullptr, do nothing */
    }
    else if (bytes_left > sizeof(buffer))
    {
        /* Insufficient space, do nothing */
    }
    else if (!pb_read(stream, buffer, bytes_left))
    {
        /* Failed to read bytes left into buffer, do nothing */
    }
    else if (!pb_decode(&istream, controller_messages_BlockOccupancy_fields, &block_occupancy_message))
    {
        /* Failed to decode block occupancy message, do nothing */
    }
    else if (Error::ERROR_NONE != hardware_wayside_controller_handler->SetBlockOccupancy(block_occupancy_message.block, block_occupancy_message.occupied))
    {
        /* Failed to set block occupancy in wayside controller, do nothing */
    }
    else
    {
        decoded = true;
    }

    return decoded;
}

template <size_t buffer_size>
bool HardwareWaysideControllerHandler<buffer_size>::EncodeBlockStates(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    bool encoded = false;

    // TODO

    ESP_LOGI("HW WC HANDLER", "EncodeBlockStates called");

    // for (const types::BlockState &block_state : block_states)
    // {
    //     controller_messages::BlockState* block_state_message = block_states_message.add_states();
    //     block_state_message->set_block(block_state.block);
    //     block_state_message->set_occupied(block_state.occupied);
    //     block_state_message->set_track_failure(block_state.track_failure);
    // }

    return encoded;
}

template <size_t buffer_size>
Error HardwareWaysideControllerHandler<buffer_size>::GetInput(const InputId input, IoSignal &signal) const
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
Error HardwareWaysideControllerHandler<buffer_size>::SetInput(const InputId input, const IoSignal signal)
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
Error HardwareWaysideControllerHandler<buffer_size>::SetBlockOccupancy(const types::BlockId block, const bool occupied)
{
    Error    error  = Error::ERROR_INVALID_BLOCK;
    IoSignal signal = IoSignal::IOSIGNAL_HIGH;

    if (!occupied)
    {
        signal = IoSignal::IOSIGNAL_LOW;
    }

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
types::Error HardwareWaysideControllerHandler<buffer_size>::ReceiveMessages(void)
{
    types::Error                    error        = types::Error::ERROR_NONE;
    controller_network::MessageType message_type = controller_network::MESSAGETYPE_NONE;
    size_t                          message_size = controller_port_->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());

    while ((message_size > 0) && (controller_network::MESSAGETYPE_NONE != message_type))
    {
        switch (message_type)
        {
        case controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA:
            error = HandleTrackCircuitData();
            break;
        case controller_network::MESSAGETYPE_BLOCK_OCCUPANCIES:
            error = HandleBlockOccupancies();
            break;
        default:
            break;
        }

        message_size = controller_port_->ReceiveMessage(message_type, message_buffer_.data(), message_buffer_.size());
    }

    return error;
}

template <size_t buffer_size>
types::Error HardwareWaysideControllerHandler<buffer_size>::HandleTrackCircuitData(void)
{
    types::Error                          error = types::Error::ERROR_NONE;
    pb_istream_t istream = pb_istream_from_buffer(message_buffer_.data(), message_buffer_.size());
    controller_messages_TrackCircuitData track_circuit_data_message = controller_messages_TrackCircuitData_init_zero;

    if (!pb_decode(&istream, controller_messages_TrackCircuitData_fields, &track_circuit_data_message))
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    else
    {
        types::TrackCircuitData track_circuit_data(static_cast<types::TrackId>(track_circuit_data_message.track),
                                                   track_circuit_data_message.block,
                                                   track_circuit_data_message.speed_meters_per_second,
                                                   track_circuit_data_message.authority);
        
        wayside_controller_.GetCommandedSpeedAndAuthority(track_circuit_data);
        track_circuit_data_message.speed_meters_per_second = track_circuit_data.speed;
        track_circuit_data_message.authority = track_circuit_data.authority;

        pb_ostream_t ostream = pb_ostream_from_buffer(message_buffer_.data(), message_buffer_.size());
        
        if (!pb_encode(&ostream, controller_messages_TrackCircuitData_fields, &track_circuit_data_message))
        {
            error = types::Error::ERROR_INVALID_FORMAT;
        }
        else if (ostream.bytes_written != controller_port_->SendMessage(controller_network::MESSAGETYPE_TRACK_CIRCUIT_DATA, message_buffer_.data(), ostream.bytes_written))
        {
            error = types::Error::ERROR_INVALID_SIZE;
        }
    }


    return error;
}

template <size_t buffer_size>
types::Error HardwareWaysideControllerHandler<buffer_size>::HandleBlockOccupancies(void)
{
    types::Error                          error = types::Error::ERROR_NONE;
    pb_istream_t istream = pb_istream_from_buffer(message_buffer_.data(), message_buffer_.size());
    controller_messages_BlockOccupancies block_occupancies_message = controller_messages_BlockOccupancies_init_zero;
    block_occupancies_message.occupancies.arg = this;
    block_occupancies_message.occupancies.funcs.decode = DecodeBlockOccupancies;

    if (!pb_decode(&istream, controller_messages_BlockOccupancies_fields, &block_occupancies_message))
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }

    return error;
}

template <size_t buffer_size>
types::Error HardwareWaysideControllerHandler<buffer_size>::SendBlockStates(const std::vector<types::BlockState> &block_states)
{
    types::Error error = types::Error::ERROR_NONE;

    if (block_states.size() > 0)
    {
        pb_ostream_t ostream = pb_ostream_from_buffer(message_buffer_.data(), message_buffer_.size());
        controller_messages_BlockStates block_states_message = controller_messages_BlockStates_init_zero;

        block_states_message.track = static_cast<controller_messages_TrackId>(track_);
        block_states_message.states.arg = (void *)&block_states;
        block_states_message.states.funcs.encode = EncodeBlockStates;

        if (!pb_encode(&ostream, controller_messages_BlockStates_fields, &block_states_message))
        {
            error = types::Error::ERROR_INVALID_FORMAT;
        }
        else if (ostream.bytes_written != controller_port_->SendMessage(controller_network::MESSAGETYPE_BLOCK_STATES, message_buffer_.data(), ostream.bytes_written))
        {
            error = types::Error::ERROR_INVALID_SIZE;
        }
    }

    return error;
}

} // namespace wayside_controller

#endif // namespace TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_HANDLER_H