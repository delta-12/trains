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

WaysideController::WaysideController(std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs, std::shared_ptr<void(std::unordered_map<OutputId, bool> &inputs)> set_outputs) : get_inputs_(get_inputs), set_outputs_(set_outputs)
{
}

WaysideController::WaysideController(std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs, std::shared_ptr<void(std::unordered_map<OutputId, bool> &inputs)> set_outputs, const std::vector<BlockInput> &block_input_map) : get_inputs_(get_inputs), set_outputs_(set_outputs), block_input_map_(block_input_map)
{
}

} // namespace wayside_controller