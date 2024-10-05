#include "wayside_controller.h"

namespace wayside_controller
{

BlockState::BlockState(const types::TrackId track, const types::BlockId block, const bool occupied, const bool track_failure) : track(track), block(block), occupied(occupied), track_failure(track_failure)
{
}

TrackCircuitData::TrackCircuitData(const types::TrackId track, const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority) : track(track), block(block), speed(speed), authority(authority)
{
}

PlcInstruction::PlcInstruction(const PlcInstructionCode instruction_code, const PlcInstructionArgument argument_0, const PlcInstructionArgument argument_1, const PlcInstructionArgument argument_2) : instruction_code(instruction_code), argument_0(argument_0), argument_1(argument_1), argument_2(argument_2)
{
}

} // namespace wayside_controller