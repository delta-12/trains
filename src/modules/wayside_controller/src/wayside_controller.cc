#include "wayside_controller.h"

#include <unordered_set>

#include "logger.h"

namespace wayside_controller
{

BlockInputs::BlockInputs(void) : block(0), track_circuit_input(0), switch_input(0), has_switch(false), maintenance_mode(false), occupied(false)
{
}

BlockInputs::BlockInputs(const types::BlockId block, const InputId track_circuit_input, const InputId switch_input, const bool has_switch, const bool maintenance_mode, const bool occupied)
    : block(block), track_circuit_input(track_circuit_input), switch_input(switch_input), has_switch(has_switch), maintenance_mode(maintenance_mode), occupied(occupied)
{
}

BlockConnection::BlockConnection(void)
{
}

BlockConnection::BlockConnection(const types::BlockId from_block, const types::BlockId to_block) : from_block(from_block), to_block(to_block)
{
}

Configuration::Configuration(void)
{
}

Configuration::Configuration(const std::vector<BlockInputs> &block_input_map, const std::vector<BlockConnection> &connected_blocks) : block_input_map(block_input_map), connected_blocks(connected_blocks)
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

WaysideController::WaysideController(std::function<Error(const InputId input, IoSignal &signal)> get_input, const Configuration &configuration) : get_input_(get_input)
{
    // TODO NNF-174 error handling
    Configure(configuration);
}

Error WaysideController::Configure(const Configuration &configuration)
{
    Error                       error = ERROR_NONE;
    std::unordered_set<InputId> mapped_inputs;

    block_input_map_.clear();
    block_layout_.Clear();

    // Map block inputs
    for (const BlockInputs &block_inputs : configuration.block_input_map)
    {
        // Validate block and track circuit input
        if (block_input_map_.contains(block_inputs.block))
        {
            error = ERROR_DUPLICATE_BLOCK;
        }
        else if (!IsTrackCircuitInputValid(block_inputs.track_circuit_input))
        {
            error = ERROR_INVALID_INPUT;
        }
        else if (mapped_inputs.contains(block_inputs.track_circuit_input))
        {
            error = ERROR_DUPLICATE_INPUT;
        }

        // Validate switch input
        if ((ERROR_NONE == error) && (true == block_inputs.has_switch))
        {
            if (!IsSwitchInputValid(block_inputs.switch_input))
            {
                error = ERROR_INVALID_INPUT;
            }
            else if (mapped_inputs.contains(block_inputs.switch_input))
            {
                error = ERROR_DUPLICATE_INPUT;
            }
        }

        if (ERROR_NONE == error)
        {
            block_input_map_[block_inputs.block] = block_inputs;
            mapped_inputs.insert(block_inputs.track_circuit_input);

            if (block_inputs.has_switch)
            {
                mapped_inputs.insert(block_inputs.switch_input);
            }
        }
        else
        {
            break;
        }
    }

    // Build track layout
    if (ERROR_NONE == error)
    {
        for (const BlockConnection &block_connection : configuration.connected_blocks)
        {
            if ((!block_input_map_.contains(block_connection.from_block)) ||
                (!block_input_map_.contains(block_connection.to_block)) ||
                (!block_layout_.AddEdge(block_connection.from_block, block_connection.to_block, 1)))
            {
                error = ERROR_INVALID_BLOCK;
                break;
            }
        }
    }

    if (ERROR_NONE != error)
    {
        block_input_map_.clear();
        block_layout_.Clear();
    }

    return error;
}

Error WaysideController::GetCommandedSpeedAndAuthority(types::TrackCircuitData &track_circuit_data)
{
    Error                  error     = ERROR_NONE;
    types::MetersPerSecond speed     = 0;
    size_t                 authority = 0;

    if (!block_input_map_.contains(track_circuit_data.block))
    {
        error = ERROR_INVALID_BLOCK;
    }
    else
    {
        // TODO NNF-144 check for safe speed

        // BFS should return set of blocks in the order they can be traversed from the starting block
        std::unordered_set<types::BlockId> blocks = block_layout_.BreadthFirstSearch(track_circuit_data.block);
        blocks.erase(track_circuit_data.block);

        // Authority can only be less than or equal to initial value
        while ((authority < track_circuit_data.authority) && (false == blocks.empty()))
        {
            IoSignal track_circuit_signal = IOSIGNAL_HIGH;

            // Check for occupancy by getting track circuit input
            if ((ERROR_NONE != get_input_(block_input_map_[*blocks.begin()].track_circuit_input, track_circuit_signal)) || (IOSIGNAL_HIGH == track_circuit_signal))
            {
                break;
            }
            else
            {
                track_circuit_data.authority++;
            }

            blocks.erase(blocks.begin());
        }
    }

    track_circuit_data.speed     = speed;
    track_circuit_data.authority = authority;

    return error;
}

Error WaysideController::SetMaintenanceMode(const types::BlockId block, const bool maintenance_mode)
{
    Error error = ERROR_NONE;

    if (!block_input_map_.contains(block))
    {
        error = ERROR_INVALID_BLOCK;
    }
    else
    {
        // TODO NNF-105 can only be set if block has switch?
        block_input_map_[block].maintenance_mode = maintenance_mode;
    }

    return error;
}

Error WaysideController::SetSwitch(const types::BlockId block, const bool switch_state)
{
    // TODO NNF-105 can be used in both auto and maintenance mode?

    Error error = ERROR_NONE;

    // TODO NNF-105 only blocks with swithes can be put into maintenance mode?
    if ((!block_input_map_.contains(block)) || (false == block_input_map_[block].has_switch))
    {
        error = ERROR_INVALID_BLOCK;
    }
    else if (!IsSwitchInputValid(block_input_map_[block].switch_input))
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