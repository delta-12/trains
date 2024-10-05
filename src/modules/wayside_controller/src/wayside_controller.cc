#include "wayside_controller.h"

#include <algorithm>

namespace wayside_controller
{

BlockState::BlockState(const types::BlockId block, const bool occupied, const bool track_failure) : block(block), occupied(occupied), track_failure(track_failure)
{
}

BlockInput::BlockInput(const types::BlockId block, InputId input, const bool state, const bool maintenance_mode) : block(block), input(input), state(state), maintenance_mode(maintenance_mode)
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

WaysideController::WaysideController(std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs, std::shared_ptr<Error(const OutputId output, const bool state)> set_output) : get_inputs_(get_inputs), set_output_(set_output)
{
}

WaysideController::WaysideController(std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs, std::shared_ptr<Error(const OutputId output, const bool state)> set_output, const std::vector<BlockInput> &block_input_map) : get_inputs_(get_inputs), set_output_(set_output), block_input_map_(block_input_map)
{
}

void WaysideController::SetBlockMap(const std::vector<BlockInput> &block_input_map)
{
    block_input_map_ = block_input_map;
}

void WaysideController::SetOutput(const OutputId output, const bool state)
{
    // TODO check outputs corresponding to switches to verify safety
    (*set_output_)(output, state);
}

Error WaysideController::GetInput(const InputId input, bool &state)
{
    Error                                       error          = ERROR_INVALID_INPUT;
    std::unordered_map<InputId, bool>::iterator input_iterator = inputs_.find(input);

    if (inputs_.end() != input_iterator)
    {
        state = input_iterator->second;
        error = ERROR_NONE;
    }

    return error;
}

void WaysideController::ScanInputs(void)
{
    (*get_inputs_)(inputs_);
}

types::Error WaysideController::GetCommandedSpeedAndAuthority(TrackCircuitData &track_circuit_data)
{
    // TODO check for safe speed and authority

    return types::ERROR_INVALID_BLOCK;
}

types::Error WaysideController::SetMaintenanceMode(const types::BlockId block, const bool maintenance_mode)
{
    types::Error error = types::ERROR_INVALID_BLOCK;

    std::vector<BlockInput>::iterator block_input_iterator = std::find_if(block_input_map_.begin(), block_input_map_.end(), [block](const BlockInput &block_input)
        {
            return block_input.block == block;
        });

    if (block_input_map_.end() != block_input_iterator)
    {
        block_input_iterator->maintenance_mode = maintenance_mode;
        error                                  = types::ERROR_NONE;
    }

    return error;
}

types::Error WaysideController::SetSwitch(const types::BlockId, const bool switch_state)
{
    // TODO can be used in both auto and maintenance mode?

    return types::ERROR_INVALID_BLOCK;
}

std::vector<BlockState> WaysideController::GetBlockStates(void)
{
    // TODO

    return std::vector<BlockState>();
}

Plc::Plc(void) : program_counter_(0), registers_{} // TODO verify registers are initialized to 0
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
    bool success = false;

    // TODO

    return success;
}

bool Plc::WriteSignal(WaysideController &wayside_controller, const PlcInstructionArgument register_number, const PlcInstructionArgument output)
{
    bool success = false;

    // TODO

    return success;
}


} // namespace wayside_controller