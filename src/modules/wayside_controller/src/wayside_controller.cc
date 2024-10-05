#include "wayside_controller.h"

namespace wayside_controller
{

BlockState::BlockState(const types::BlockId block, const bool occupied, const bool track_failure) : block(block), occupied(occupied), track_failure(track_failure)
{
}

BlockInputs::BlockInputs(void) : block(0), track_circuit_input(0), switch_input(0), has_switch(false), maintenance_mode(false)
{
}

BlockInputs::BlockInputs(const types::BlockId block, const InputId track_circuit_input, const InputId switch_input, const bool has_switch, const bool maintenance_mode) : block(block), track_circuit_input(track_circuit_input), switch_input(switch_input), has_switch(has_switch), maintenance_mode(maintenance_mode)
{
}

TrackCircuitData::TrackCircuitData(const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority) : block(block), speed(speed), authority(authority)
{
}

PlcInstruction::PlcInstruction(void) : instruction_code(PLCINSTRUCTIONCODE_NOOP), argument_0(0), argument_1(0), argument_2(0)
{
}

PlcInstruction::PlcInstruction(const PlcInstructionCode instruction_code, const PlcInstructionArgument argument_0, const PlcInstructionArgument argument_1, const PlcInstructionArgument argument_2) : instruction_code(instruction_code), argument_0(argument_0), argument_1(argument_1), argument_2(argument_2)
{
}

WaysideController::WaysideController(std::function<void(std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> &inputs)> get_inputs, std::function<Error(const OutputId output, const bool state)> set_output) : get_inputs_(get_inputs), set_output_(set_output), inputs_{}     // TODO verify registers are initialized to 0
{
}

WaysideController::WaysideController(std::function<void(std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> &inputs)> get_inputs, std::function<Error(const OutputId output, const bool state)> set_output, const std::vector<BlockInputs> &block_inputs_map) : get_inputs_(get_inputs), set_output_(set_output), inputs_{}
{
    // TODO NNF-174 error handling
    SetBlockMap(block_inputs_map);
}

Error WaysideController::SetBlockMap(const std::vector<BlockInputs> &block_inputs_map)
{
    Error error = ERROR_NONE;

    block_inputs_map_.clear();

    for (const BlockInputs &block_inputs : block_inputs_map)
    {
        if (block_inputs.has_switch)
        {
            if (!IsSwitchInputValid(block_inputs.switch_input))
            {
                error = ERROR_INVALID_INPUT;
            }
        }

        if ((ERROR_NONE == error) && (true == IsTrackCircuitInputValid(block_inputs.track_circuit_input)))
        {
            block_inputs_map_[block_inputs.block] = block_inputs;
        }
        else
        {
            error = ERROR_INVALID_INPUT;
            block_inputs_map_.clear();
            break;
        }
    }

    return error;
}

Error WaysideController::SetOutput(const OutputId output, const bool state)
{
    // TODO NNF-105 check outputs corresponding to switches to verify safety

    return set_output_(output, state);
}

Error WaysideController::GetInput(const InputId input, bool &state)
{
    Error error = ERROR_INVALID_INPUT;

    if (input < inputs_.size())
    {
        state = inputs_[input];
        error = ERROR_NONE;
    }

    return error;
}

void WaysideController::ScanInputs(void)
{
    get_inputs_(inputs_);
}

types::Error WaysideController::GetCommandedSpeedAndAuthority(TrackCircuitData &track_circuit_data)
{
    types::Error error = types::ERROR_INVALID_BLOCK;

    std::unordered_map<types::BlockId, BlockInputs>::iterator block_inputs = block_inputs_map_.find(track_circuit_data.block);

    if (block_inputs_map_.end() != block_inputs)
    {
        // TODO NNF-144, NNF-145 check for safe speed and authority

        error = types::ERROR_NONE;
    }

    return error;
}

types::Error WaysideController::SetMaintenanceMode(const types::BlockId block, const bool maintenance_mode)
{
    types::Error error = types::ERROR_INVALID_BLOCK;

    std::unordered_map<types::BlockId, BlockInputs>::iterator block_inputs = block_inputs_map_.find(block);

    if (block_inputs_map_.end() != block_inputs)
    {
        block_inputs->second.maintenance_mode = maintenance_mode;
        error                                 = types::ERROR_NONE;
    }

    return error;
}

types::Error WaysideController::SetSwitch(const types::BlockId block, const bool switch_state)
{
    // TODO NNF-105 can be used in both auto and maintenance mode?

    types::Error error = types::ERROR_INVALID_BLOCK;

    std::unordered_map<types::BlockId, BlockInputs>::iterator block_inputs = block_inputs_map_.find(block);

    if ((block_inputs_map_.end() != block_inputs) &&
        (true == block_inputs->second.has_switch) &&
        (true == IsSwitchInputValid(block_inputs->second.switch_input)))
    {
        inputs_[block_inputs->second.switch_input] = switch_state;
        error                                      = types::ERROR_NONE;
    }

    return error;
}

std::vector<BlockState> WaysideController::GetBlockStates(void)
{
    // TODO NNF-168

    return std::vector<BlockState>();
}

bool WaysideController::IsTrackCircuitInputValid(const InputId input) const
{
    return (input < WAYSIDE_CONTROLLER_PHYSICAL_INPUT_COUNT);
}

bool WaysideController::IsSwitchInputValid(const InputId input) const
{
    return ((input >= WAYSIDE_CONTROLLER_PHYSICAL_INPUT_COUNT) && (input < WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT));
}

Plc::Plc(void) : program_counter_(0), registers_{}     // TODO NNF-104 verify registers are initialized to 0
{
}

Plc::Plc(const std::vector<PlcInstruction> &instructions) : instructions_(instructions), program_counter_(0), registers_{}
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

bool Plc::Run(WaysideController &wayside_controller)
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
            if (ReadSignal(wayside_controller, instruction.argument_0, instruction.argument_1))
            {
                program_counter_++;
            }
            else
            {
                execution_success = false;
            }
            break;
        case PLCINSTRUCTIONCODE_WRITE_SIGNAL:
            if (WriteSignal(wayside_controller, instruction.argument_0, instruction.argument_1))
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

bool Plc::ReadSignal(WaysideController &wayside_controller, const PlcInstructionArgument register_number, const PlcInstructionArgument input)
{
    bool success        = false;
    bool register_value = false;

    if (ERROR_NONE == wayside_controller.GetInput(input, register_value))
    {
        if (register_value)
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

bool Plc::WriteSignal(WaysideController &wayside_controller, const PlcInstructionArgument register_number, const PlcInstructionArgument output)
{
    bool success      = false;
    bool output_value = false;

    if (0 != registers_[register_number])
    {
        output_value = true;
    }

    if (ERROR_NONE == wayside_controller.SetOutput(output, output_value))
    {
        success = true;
    }

    return success;
}

} // namespace wayside_controller