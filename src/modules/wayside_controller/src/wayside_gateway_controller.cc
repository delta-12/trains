#include "wayside_controller_gateway.h"

#include "logger.h"

namespace wayside_controller
{

void Gateway::UpdateControllers(void)
{
}

types::Error Gateway::UpdateTrackModel(std::shared_ptr<track_model::TrackModel> track)
{
    // TODO NNF-172 receive data
    LOGGER_UNUSED(track); // temporary fix to remove compiler warnings

    return types::ERROR_INVALID_TRACK;
}

types::Error Gateway::UpdateTrackModels(std::vector<std::shared_ptr<track_model::TrackModel>> &tracks)
{
    // TODO NNF-172
    LOGGER_UNUSED(tracks); // temporary fix to remove compiler warnings

    return types::ERROR_INVALID_TRACK;
}

void Gateway::SetSimulationMode(const bool simulation)
{
    // TODO NNF-172
    LOGGER_UNUSED(simulation); // temporary fix to remove compiler warnings
}

bool Gateway::GetSimulationMode(void) const
{
    // TODO NNF-172

    return false;
}

void Gateway::AttachPort(std::shared_ptr<types::Port> port)
{
    // TODO NNF-172
    LOGGER_UNUSED(port); // temporary fix to remove compiler warnings
}

types::Error Gateway::SetSuggestedSpeedAndAuthority(const types::TrackId track, const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority)
{
    // TODO NNF-172 serialize data, lookup wayside corresponding to track and block, send data
    LOGGER_UNUSED(track);     // temporary fix to remove compiler warnings
    LOGGER_UNUSED(block);     // temporary fix to remove compiler warnings
    LOGGER_UNUSED(speed);     // temporary fix to remove compiler warnings
    LOGGER_UNUSED(authority); // temporary fix to remove compiler warnings

    return types::ERROR_INVALID_TRACK;
}

types::Error Gateway::SetMaintenanceMode(const types::TrackId track, const types::BlockId block, const bool maintenance)
{
    // TODO NNF-172
    LOGGER_UNUSED(track);       // temporary fix to remove compiler warnings
    LOGGER_UNUSED(block);       // temporary fix to remove compiler warnings
    LOGGER_UNUSED(maintenance); // temporary fix to remove compiler warnings

    return types::ERROR_INVALID_TRACK;
}

types::Error Gateway::SetSwitchState(const types::TrackId track, const types::BlockId block, const bool switch_state)
{
    // TODO NNF-172
    LOGGER_UNUSED(track);        // temporary fix to remove compiler warnings
    LOGGER_UNUSED(block);        // temporary fix to remove compiler warnings
    LOGGER_UNUSED(switch_state); // temporary fix to remove compiler warnings

    return types::ERROR_INVALID_TRACK;
}

void Gateway::GetBlockStates(const types::TrackId track, std::vector<BlockState> &states)
{
    // TODO NNF-172
    LOGGER_UNUSED(track);  // temporary fix to remove compiler warnings
    LOGGER_UNUSED(states); // temporary fix to remove compiler warnings
}


} // namespace wayside_controller