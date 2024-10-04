#include "wayside_controller.h"

namespace wayside_controller
{

BlockState::BlockState(const types::TrackId track, const types::BlockId block, const bool occupied, const bool track_failure) : track(track), block(block), occupied(occupied), track_failure(track_failure)
{
}

TrackCircuitData::TrackCircuitData(const types::TrackId track, const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority) : track(track), block(block), speed(speed), authority(authority)
{
}

} // namespace wayside_controller