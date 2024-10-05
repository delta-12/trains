#include "wayside_controller.h"

namespace wayside_controller
{

BlockState::BlockState(const types::BlockId block, const bool occupied, const bool track_failure) : block(block), occupied(occupied), track_failure(track_failure)
{
}

BlockInput::BlockInput(const types::BlockId block, InputId input, const bool state) : block(block), input(input), state(state)
{
}

TrackCircuitData::TrackCircuitData(const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority) : block(block), speed(speed), authority(authority)
{
}

PlcInstruction::PlcInstruction(const PlcInstructionCode instruction_code, const PlcInstructionArgument argument_0, const PlcInstructionArgument argument_1, const PlcInstructionArgument argument_2) : instruction_code(instruction_code), argument_0(argument_0), argument_1(argument_1), argument_2(argument_2)
{
}

WaysideController::WaysideController(std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs, std::shared_ptr<types::Error(const OutputId output, const bool state)> set_output) : get_inputs_(get_inputs), set_output_(set_output)
{
}

WaysideController::WaysideController(std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs, std::shared_ptr<types::Error(const OutputId output, const bool state)> set_output, const std::vector<BlockInput> &block_input_map) : get_inputs_(get_inputs), set_output_(set_output), block_input_map_(block_input_map)
{
}

void WaysideController::SetBlockMap(const std::vector<BlockInput> &block_input_map)
{
    block_input_map_ = block_input_map;
}

void WaysideController::SetOutput(const OutputId output, const bool state)
{
    (*set_output_)(output, state);
}
// check outputs corresponding to switches to verify safety
void WaysideController::GetInput(const InputId input, bool &state)
{
    // TODO
}

void WaysideController::ScanInputs(void)
{
    // TODO
}

types::Error WaysideController::GetCommandedSpeedAndAuthority(TrackCircuitData &track_circuit_data)
{
    // TODO
}
// check for safe speed and authority
void WaysideController::SetManualMode(const types::BlockId block, const bool manual_mode)
{
    // TODO
}

void WaysideController::SetSwitch(const types::BlockId, const bool switch_state)
{
    // TODO
}
// can be used in both auto and maintenance mode?
std::vector<BlockState> WaysideController::GetBlockStates(void)
{
    // TODO
}


} // namespace wayside_controller