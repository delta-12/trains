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
}

std::vector<BlockState> WaysideController::GetBlockStates(void)
{
    // TODO
}


} // namespace wayside_controller