#include "wayside_controller.h"

#include <unordered_set>

namespace wayside_controller
{

BlockIo::BlockIo(void) : block(0), track_circuit_input(0), switch_input(0), has_switch(false), maintenance_mode(false)
{
}

BlockIo::BlockIo(const types::BlockId block, const InputId track_circuit_input, const InputId switch_input, const bool has_switch, const bool maintenance_mode) : block(block), track_circuit_input(track_circuit_input), switch_input(switch_input), has_switch(has_switch), maintenance_mode(maintenance_mode)
{
}

PlcInstruction::PlcInstruction(void) : instruction_code(PLCINSTRUCTIONCODE_NOOP), argument_0(0), argument_1(0), argument_2(0)
{
}

PlcInstruction::PlcInstruction(const PlcInstructionCode instruction_code, const PlcInstructionArgument argument_0, const PlcInstructionArgument argument_1, const PlcInstructionArgument argument_2) : instruction_code(instruction_code), argument_0(argument_0), argument_1(argument_1), argument_2(argument_2)
{
}

WaysideController::WaysideController(std::function<Error(const InputId input, IoSignal &signal)> get_input) : get_input_(get_input)
{
}

WaysideController::WaysideController(std::function<Error(const InputId input, IoSignal &signal)> get_input, const std::vector<BlockIo> &block_io_map) : get_input_(get_input)
{
    // TODO NNF-174 error handling
    SetBlockMap(block_io_map);
}

Error WaysideController::SetBlockMap(const std::vector<BlockIo> &block_io_map)
{
    Error                       error = ERROR_NONE;
    std::unordered_set<InputId> mapped_inputs;

    block_io_map_.clear();

    for (const BlockIo &block_inputs : block_io_map)
    {
        if (block_io_map_.end() != block_io_map_.find(block_inputs.block))
        {
            error = ERROR_DUPLICATE_BLOCK;
        }
        else if (!IsTrackCircuitInputValid(block_inputs.track_circuit_input))
        {
            error = ERROR_INVALID_INPUT;
        }
        else if (mapped_inputs.end() != mapped_inputs.find(block_inputs.track_circuit_input))
        {
            error = ERROR_DUPLICATE_INPUT;
        }

        if ((ERROR_NONE == error) && (true == block_inputs.has_switch))
        {
            if (!IsSwitchInputValid(block_inputs.switch_input))
            {
                error = ERROR_INVALID_INPUT;
            }
            else if (mapped_inputs.end() != mapped_inputs.find(block_inputs.switch_input))
            {
                error = ERROR_DUPLICATE_INPUT;
            }
        }

        // TODO check for duplicate outputs

        if (ERROR_NONE == error)
        {
            block_io_map_[block_inputs.block] = block_inputs;
            mapped_inputs.insert(block_inputs.track_circuit_input);

            if (block_inputs.has_switch)
            {
                mapped_inputs.insert(block_inputs.switch_input);
            }
        }
        else
        {
            block_io_map_.clear();
            break;
        }
    }

    return error;
}

Error WaysideController::GetCommandedSpeedAndAuthority(types::TrackCircuitData &track_circuit_data)
{
    Error error = ERROR_NONE;

    std::unordered_map<types::BlockId, BlockIo>::iterator block_inputs = block_io_map_.find(track_circuit_data.block);

    if (block_io_map_.end() == block_inputs)
    {
        error = ERROR_INVALID_BLOCK;
    }
    else
    {
        // TODO NNF-144 check for safe speed

        // TODO NNF-145 authority
    }

    return error;
}

Error WaysideController::SetMaintenanceMode(const types::BlockId block, const bool maintenance_mode)
{
    Error error = ERROR_NONE;

    std::unordered_map<types::BlockId, BlockIo>::iterator block_inputs = block_io_map_.find(block);

    if (block_io_map_.end() == block_inputs)
    {
        error = ERROR_INVALID_BLOCK;
    }
    else
    {
        // TODO NNF-105 can only be set if block has switch?
        block_inputs->second.maintenance_mode = maintenance_mode;
    }

    return error;
}

Error WaysideController::SetSwitch(const types::BlockId block, const bool switch_state)
{
    // TODO NNF-105 can be used in both auto and maintenance mode?

    Error error = ERROR_NONE;

    std::unordered_map<types::BlockId, BlockIo>::iterator block_inputs = block_io_map_.find(block);

    // TODO NNF-105 only blocks with swithes can be put into maintenance mode?
    if ((block_io_map_.end() == block_inputs) || (false == block_inputs->second.has_switch))
    {
        error = ERROR_INVALID_BLOCK;
    }
    else if (!IsSwitchInputValid(block_inputs->second.switch_input))
    {
        error = ERROR_INVALID_INPUT;
    }
    else
    {
        // TODO NNF-105 verify safe switch state here or in PLC program
        // TODO NNF-105 set virtual input accordingly
    }

    return error;
}

std::vector<types::BlockState> WaysideController::GetBlockStates(void)
{
    // TODO NNF-168

    return std::vector<types::BlockState>();
}

bool WaysideController::IsTrackCircuitInputValid(const InputId input) const
{
    return (input < WAYSIDE_CONTROLLER_PHYSICAL_INPUT_COUNT);
}

bool WaysideController::IsSwitchInputValid(const InputId input) const
{
    return ((input >= WAYSIDE_CONTROLLER_PHYSICAL_INPUT_COUNT) && (input < WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT));
}

Plc::Plc(std::function<Error(const InputId input, IoSignal &signal)> get_input, std::function<Error(const OutputId output, const IoSignal signal)> set_output) : get_input_(get_input), set_output_(set_output), program_counter_(0), registers_{}     // TODO NNF-104 verify registers are initialized to 0
{
}

Plc::Plc(std::function<Error(const InputId input, IoSignal &signal)> get_input, std::function<Error(const OutputId output, const IoSignal signal)> set_output, const std::vector<PlcInstruction> &instructions) : get_input_(get_input), set_output_(set_output), instructions_(instructions), program_counter_(0), registers_{}
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