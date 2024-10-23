#include "wayside_controller.h"

#include <unordered_set>

#include "logger.h"

namespace wayside_controller
{

WaysideBlock::WaysideBlock(void) : block(0), primary_connection(0), switch_connection(0), track_circuit_input(0), switch_input(0), has_switch(false),
    maintenance_mode(false), occupied(false)
{
}

WaysideBlock::WaysideBlock(const types::BlockId block, const types::BlockId primary_connection, const types::BlockId switch_connection, const types::BlockDirection direction,
                           const InputId track_circuit_input, const InputId switch_input, const bool has_switch, const bool maintenance_mode, const bool occupied)
    : block(block), primary_connection(primary_connection), switch_connection(switch_connection), direction(direction), track_circuit_input(track_circuit_input),
    switch_input(switch_input), has_switch(has_switch), maintenance_mode(maintenance_mode), occupied(occupied)
{
}

PlcInstruction::PlcInstruction(void) : instruction_code(PLCINSTRUCTIONCODE_NOOP), argument_0(0), argument_1(0), argument_2(0)
{
}

PlcInstruction::PlcInstruction(const PlcInstructionCode instruction_code, const PlcInstructionArgument argument_0, const PlcInstructionArgument argument_1, const PlcInstructionArgument argument_2)
    : instruction_code(instruction_code), argument_0(argument_0), argument_1(argument_1), argument_2(argument_2)
{
}

WaysideController::WaysideController(std::function<Error(const InputId input, IoSignal &signal)> get_input) : get_input_(get_input)
{
}

WaysideController::WaysideController(std::function<Error(const InputId input, IoSignal &signal)> get_input, const std::vector<WaysideBlock> &blocks) : get_input_(get_input)
{
    // TODO NNF-174 error handling
    Configure(blocks);
}

Error WaysideController::Configure(const std::vector<WaysideBlock> &blocks)
{
    Error                       error = ERROR_NONE;
    std::unordered_set<InputId> mapped_inputs;

    block_configuration_.clear();
    block_layout_.Clear();

    // Map block inputs
    for (const WaysideBlock &wayside_block : blocks)
    {
        // Validate block and track circuit input
        if (TYPES_END_BLOCK_ID == wayside_block.block)
        {
            error = ERROR_INVALID_BLOCK;
        }
        else if (block_configuration_.contains(wayside_block.block))
        {
            error = ERROR_DUPLICATE_BLOCK;
        }
        else if (!IsTrackCircuitInputValid(wayside_block.track_circuit_input))
        {
            error = ERROR_INVALID_INPUT;
        }
        else if (mapped_inputs.contains(wayside_block.track_circuit_input))
        {
            error = ERROR_DUPLICATE_INPUT;
        }

        // Validate switch input
        if ((ERROR_NONE == error) && (true == wayside_block.has_switch))
        {
            if (!IsSwitchInputValid(wayside_block.switch_input))
            {
                error = ERROR_INVALID_INPUT;
            }
            else if (mapped_inputs.contains(wayside_block.switch_input))
            {
                error = ERROR_DUPLICATE_INPUT;
            }
        }

        if (ERROR_NONE == error)
        {
            block_configuration_[wayside_block.block] = wayside_block;
            mapped_inputs.insert(wayside_block.track_circuit_input);
            block_layout_.AddEdge(wayside_block.block, wayside_block.primary_connection, 1);

            if (types::BLOCKDIRECTION_BIDIRECTIONAL == wayside_block.direction)
            {
                block_layout_.AddEdge(wayside_block.primary_connection, wayside_block.block, 1);
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

    if (ERROR_NONE != error)
    {
        block_configuration_.clear();
        block_layout_.Clear();
    }

    return error;
}

Error WaysideController::GetCommandedSpeedAndAuthority(types::TrackCircuitData &track_circuit_data)
{
    Error                  error     = ERROR_NONE;
    types::MetersPerSecond speed     = 0;
    size_t                 authority = 0;

    if (false == block_configuration_.contains(track_circuit_data.block))
    {
        error = ERROR_INVALID_BLOCK;
    }
    else
    {
        // TODO NNF-144 check for safe speed

        // BFS should return set of blocks in the order they can be traversed from the starting block
        std::vector<types::BlockId>                 blocks         = block_layout_.BreadthFirstSearch(track_circuit_data.block);
        std::vector<types::BlockId>::const_iterator selected_block = blocks.begin() + 1;     // Don't include current block in authority

        // Authority can only be less than or equal to initial value
        while ((authority < track_circuit_data.authority) && (blocks.end() != selected_block))
        {
            if (TYPES_END_BLOCK_ID == *selected_block)
            {
                // Train cannot travel further than the end of the track, should be the end of blocks
                // Do nothing
            }
            else if (!block_configuration_.contains(*selected_block))
            {
                error = ERROR_INVALID_BLOCK;
            }
            else
            {
                // Occupancy check
                IoSignal track_circuit_signal = IOSIGNAL_HIGH;
                error = get_input_(block_configuration_[*selected_block].track_circuit_input, track_circuit_signal);

                if ((ERROR_NONE != error) || (IOSIGNAL_HIGH == track_circuit_signal))
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
    Error error = ERROR_NONE;

    if (!block_configuration_.contains(block))
    {
        error = ERROR_INVALID_BLOCK;
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

    Error error = ERROR_NONE;

    // TODO NNF-105 only blocks with swithes can be put into maintenance mode?
    if ((!block_configuration_.contains(block)) || (false == block_configuration_[block].has_switch))
    {
        error = ERROR_INVALID_BLOCK;
    }
    else if (!IsSwitchInputValid(block_configuration_[block].switch_input))
    {
        error = ERROR_INVALID_INPUT;
    }
    else
    {
        // TODO NNF-105 verify safe switch state here or in PLC program
        // TODO NNF-105 set virtual input accordingly
        // TODO NNF-105 update block_layout_ graph accordingly (i.e. add/remove connections betweens blocks)
        LOGGER_UNUSED(switch_state);
    }

    return error;
}

std::vector<types::BlockState> WaysideController::GetBlockStates(void)
{
    // TODO NNF-168

    return std::vector<types::BlockState>();
}

bool WaysideController::IsTrackCircuitInputValid(const InputId input)
{
    return (input < WAYSIDE_CONTROLLER_PHYSICAL_INPUT_COUNT);
}

bool WaysideController::IsSwitchInputValid(const InputId input)
{
    return ((input >= WAYSIDE_CONTROLLER_PHYSICAL_INPUT_COUNT) && (input < WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT));
}

Plc::Plc(std::function<Error(const InputId input, IoSignal &signal)> get_input, std::function<Error(const OutputId output, const IoSignal signal)> set_output)
    : get_input_(get_input), set_output_(set_output), program_counter_(0), registers_{}     // TODO NNF-104 verify registers are initialized to 0
{
}

Plc::Plc(std::function<Error(const InputId input, IoSignal &signal)> get_input, std::function<Error(const OutputId output, const IoSignal signal)> set_output, const std::vector<PlcInstruction> &instructions)
    : get_input_(get_input), set_output_(set_output), instructions_(instructions), program_counter_(0), registers_{}
{
}

void Plc::SetInstructions(const std::vector<PlcInstruction> &instructions)
{
    instructions_ = instructions;
}

uint32_t Plc::GetProgramCounter(void) const
{
    return program_counter_;
}

PlcInstruction Plc::GetInstruction(void) const
{
    return instructions_[program_counter_];
}

bool Plc::Run(void)
{
    PlcInstruction instruction;
    bool           execution_success = true;

    while ((program_counter_ < instructions_.size()) && (true == execution_success))
    {
        instruction = instructions_[program_counter_];

        switch (instruction.instruction_code)
        {
        case PLCINSTRUCTIONCODE_NOOP:
            program_counter_++;
            break;
        case PLCINSTRUCTIONCODE_READ_IMMEDIATE:
            registers_[instruction.argument_0] = instruction.argument_1;
            program_counter_++;
            break;
        case PLCINSTRUCTIONCODE_READ_SIGNAL:
            if (ReadSignal(instruction.argument_0, instruction.argument_1))
            {
                program_counter_++;
            }
            else
            {
                execution_success = false;
            }
            break;
        case PLCINSTRUCTIONCODE_WRITE_SIGNAL:
            if (WriteSignal(instruction.argument_0, instruction.argument_1))
            {
                program_counter_++;
            }
            else
            {
                execution_success = false;
            }
            break;
        case PLCINSTRUCTIONCODE_EQUALS:
            if (registers_[instruction.argument_1] == registers_[instruction.argument_2])
            {
                registers_[instruction.argument_0] = 1;
            }
            else
            {
                registers_[instruction.argument_0] = 0;
            }
            program_counter_++;
            break;
        case PLCINSTRUCTIONCODE_OR:
            if ((0 == registers_[instruction.argument_1]) && (0 == registers_[instruction.argument_2]))
            {
                registers_[instruction.argument_0] = 0;
            }
            else
            {
                registers_[instruction.argument_0] = 1;
            }
            program_counter_++;
            break;
        case PLCINSTRUCTIONCODE_BRANCH_IF:
            if (registers_[instruction.argument_0] == 0)
            {
                program_counter_ = instruction.argument_2;
            }
            else
            {
                program_counter_ = instruction.argument_1;
            }
            break;
        case PLCINSTRUCTIONCODE_BRANCH_UNDCONDITIONAL:
            program_counter_ = instruction.argument_0;
            break;
        default:
            execution_success = false;
            break;
        }
    }

    return execution_success;
}

bool Plc::ReadSignal(const PlcInstructionArgument register_number, const PlcInstructionArgument input)
{
    bool     success = false;
    IoSignal signal  = IOSIGNAL_LOW;

    if (ERROR_NONE == get_input_(input, signal))
    {
        if (IOSIGNAL_HIGH == signal)
        {
            registers_[register_number] = 1;
        }
        else
        {
            registers_[register_number] = 0;
        }

        success = true;
    }

    return success;
}

bool Plc::WriteSignal(const PlcInstructionArgument register_number, const PlcInstructionArgument output)
{
    bool     success = false;
    IoSignal signal  = IOSIGNAL_LOW;

    if (0 != registers_[register_number])
    {
        signal = IOSIGNAL_HIGH;
    }

    if (ERROR_NONE == set_output_(output, signal))
    {
        success = true;
    }

    return success;
}

} // namespace wayside_controller