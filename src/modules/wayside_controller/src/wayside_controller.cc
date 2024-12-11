#include "wayside_controller.h"

#include <unordered_set>

namespace wayside_controller
{

WaysideBlock::WaysideBlock(void) : block(0), next_block(0), primary_connection(0), secondary_connection(0), track_circuit_input(0), switch_input(0), has_switch(false),
    maintenance_mode(false), occupancy_signal(IoSignal::IOSIGNAL_LOW)
{
}

WaysideBlock::WaysideBlock(const types::BlockId block, const types::BlockId next_block, const types::BlockId primary_connection, const types::BlockId secondary_connection,
                           const types::BlockDirection direction, const InputId track_circuit_input, const InputId switch_input, const OutputId switch_output,
                           const bool has_switch, const bool maintenance_mode, const IoSignal occupancy_signal)
    : block(block), next_block(next_block), primary_connection(primary_connection), secondary_connection(secondary_connection), direction(direction),
    track_circuit_input(track_circuit_input), switch_input(switch_input), switch_output(switch_output), has_switch(has_switch), maintenance_mode(maintenance_mode),
    occupancy_signal(occupancy_signal)
{
}

WaysideController::WaysideController(const std::function<Error(const InputId input, IoSignal &signal)> &get_input) : get_input_(get_input)
{
}

WaysideController::WaysideController(const std::function<Error(const InputId input, IoSignal &signal)> &get_input, const std::vector<WaysideBlock> &blocks) : get_input_(get_input)
{
    // TODO NNF-174 error handling
    Configure(blocks);
}

Error WaysideController::Configure(const std::vector<WaysideBlock> &blocks)
{
    Error                       error = Error::ERROR_NONE;
    std::unordered_set<InputId> mapped_inputs;

    block_configuration_.clear();
    block_layout_.Clear();

    // Map block inputs
    for (const WaysideBlock &wayside_block : blocks)
    {
        // Validate block and track circuit input
        if (types::kEndBlock == wayside_block.block)
        {
            error = Error::ERROR_INVALID_BLOCK;
        }
        else if (block_configuration_.contains(wayside_block.block))
        {
            error = Error::ERROR_DUPLICATE_BLOCK;
        }
        else if (!IsTrackCircuitInputValid(wayside_block.track_circuit_input))
        {
            error = Error::ERROR_INVALID_INPUT;
        }
        else if (mapped_inputs.contains(wayside_block.track_circuit_input))
        {
            error = Error::ERROR_DUPLICATE_INPUT;
        }

        // Validate switch input
        if ((Error::ERROR_NONE == error) && (true == wayside_block.has_switch))
        {
            if (!IsSwitchInputValid(wayside_block.switch_input))
            {
                error = Error::ERROR_INVALID_INPUT;
            }
            else if (mapped_inputs.contains(wayside_block.switch_input))
            {
                error = Error::ERROR_DUPLICATE_INPUT;
            }
        }

        if (Error::ERROR_NONE == error)
        {
            block_configuration_[wayside_block.block] = wayside_block;
            mapped_inputs.insert(wayside_block.track_circuit_input);
            block_layout_.AddEdge(wayside_block.block, wayside_block.next_block, 1);

            if (types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL == wayside_block.direction)
            {
                block_layout_.AddEdge(wayside_block.next_block, wayside_block.block, 1);
            }

            if (wayside_block.has_switch)
            {
                mapped_inputs.insert(wayside_block.switch_input);
            }
        }
        else
        {
            break;
        }
    }

    if (Error::ERROR_NONE != error)
    {
        block_configuration_.clear();
        block_layout_.Clear();
    }

    return error;
}

Error WaysideController::GetCommandedSpeedAndAuthority(types::TrackCircuitData &track_circuit_data)
{
    Error                  error     = Error::ERROR_NONE;
    types::MetersPerSecond speed     = 0;
    size_t                 authority = 0;

    if (false == block_configuration_.contains(track_circuit_data.block))
    {
        error = Error::ERROR_INVALID_BLOCK;
    }
    else
    {
        // TODO NNF-144 check for safe speed
        speed = track_circuit_data.speed;

        // BFS should return set of blocks in the order they can be traversed from the starting block
        std::vector<types::BlockId>                 blocks         = block_layout_.BreadthFirstSearch(track_circuit_data.block);
        std::vector<types::BlockId>::const_iterator selected_block = blocks.begin() + 1;     // Don't include current block in authority

        // Authority can only be less than or equal to initial value
        while ((authority < track_circuit_data.authority) && (blocks.end() != selected_block))
        {
            if (types::kEndBlock == *selected_block)
            {
                // Train cannot travel further than the end of the track, should be the end of blocks
                // Do nothing
            }
            else if (!block_configuration_.contains(*selected_block))
            {
                error = Error::ERROR_INVALID_BLOCK;
            }
            else
            {
                // Occupancy check
                IoSignal track_circuit_signal = IoSignal::IOSIGNAL_HIGH;
                error = get_input_(block_configuration_[*selected_block].track_circuit_input, track_circuit_signal);

                if ((Error::ERROR_NONE != error) || (IoSignal::IOSIGNAL_HIGH == track_circuit_signal))
                {
                    break;
                }
            }

            ++authority;
            ++selected_block;
        }
    }

    track_circuit_data.speed     = speed;
    track_circuit_data.authority = authority;

    return error;
}

Error WaysideController::SetMaintenanceMode(const types::BlockId block, const bool maintenance_mode)
{
    Error error = Error::ERROR_NONE;

    if (!block_configuration_.contains(block))
    {
        error = Error::ERROR_INVALID_BLOCK;
    }
    else
    {
        // TODO NNF-105 can only be set if block has switch?
        block_configuration_[block].maintenance_mode = maintenance_mode;
    }

    return error;
}

Error WaysideController::SetSwitch(const types::BlockId block, const bool switch_state)
{
    // TODO NNF-105 can be used in both auto and maintenance mode?

    Error error = Error::ERROR_NONE;

    // TODO NNF-105 only blocks with swithes can be put into maintenance mode?
    if ((!block_configuration_.contains(block)) || (false == block_configuration_[block].has_switch))
    {
        error = Error::ERROR_INVALID_BLOCK;
    }
    else if (!IsSwitchInputValid(block_configuration_[block].switch_input))
    {
        error = Error::ERROR_INVALID_INPUT;
    }
    else
    {
        // TODO NNF-105 verify safe switch state here or in PLC program
        // TODO NNF-105 set virtual input accordingly
        // TODO NNF-105 update block_layout_ graph accordingly (i.e. add/remove connections betweens blocks)
        (void)(switch_state);
    }

    return error;
}

Error WaysideController::GetBlockStates(std::vector<types::BlockState> &block_states)
{
    Error error = Error::ERROR_NONE;

    block_states.clear();

    for (std::pair<const types::BlockId, WaysideBlock> &block : block_configuration_)
    {
        IoSignal io_signal = IoSignal::IOSIGNAL_LOW;
        error = get_input_(block.second.track_circuit_input, io_signal);

        if (Error::ERROR_NONE != error)
        {
            break;
        }
        else if (io_signal != block.second.occupancy_signal)
        {
            block.second.occupancy_signal = io_signal;

            // TODO NNF-227 report track failures
            block_states.emplace_back(block.second.block, io_signal == IoSignal::IOSIGNAL_HIGH, false);
        }
    }

    return error;
}

Error WaysideController::UpdateSwitchPosition(const OutputId switch_output, const IoSignal signal)
{
    Error error = Error::ERROR_INVALID_OUTPUT;

    std::unordered_map<types::BlockId, WaysideBlock>::const_iterator block = std::ranges::find_if(block_configuration_, [&switch_output](std::pair<const types::BlockId, WaysideBlock> &block){
            return (block.second.has_switch && (switch_output == block.second.switch_output));
        });

    if (block_configuration_.end() != block)
    {
        if (IoSignal::IOSIGNAL_LOW == signal)
        {
            block_layout_.AddEdge(block->second.block, block->second.primary_connection, 1);
            block_layout_.AddEdge(block->second.primary_connection, block->second.block, 1); // TODO Direction of primary connection block unknown
            block_layout_.RemoveEdge(block->second.block, block->second.secondary_connection);
            block_layout_.RemoveEdge(block->second.secondary_connection, block->second.block); // TODO Direction of primary connection block unknown
        }
        else
        {
            block_layout_.RemoveEdge(block->second.block, block->second.primary_connection);
            block_layout_.RemoveEdge(block->second.primary_connection, block->second.block); // TODO Direction of primary connection block unknown
            block_layout_.AddEdge(block->second.block, block->second.secondary_connection, 1);
            block_layout_.AddEdge(block->second.secondary_connection, block->second.block, 1); // TODO Direction of primary connection block unknown
        }

        error = Error::ERROR_NONE;
    }

    return error;
}

bool WaysideController::IsTrackCircuitInputValid(const InputId input)
{
    return (input < kPhysicalInputs);
}

bool WaysideController::IsSwitchInputValid(const InputId input)
{
    return ((input >= kPhysicalInputs) && (input < kTotalInputs));
}

} // namespace wayside_controller