#include "wayside_controller_gateway.h"

namespace wayside_controller
{

void Gateway::UpdateControllers(void)
{
}

types::Error Gateway::UpdateTrackModel(std::shared_ptr<track_model::TrackModel> track)
{
    // TODO receive data
}

types::Error Gateway::UpdateTrackModels(std::vector<std::shared_ptr<track_model::TrackModel> > &tracks)
{
}

void Gateway::SetSimulationMode(const bool simulation)
{
    // TODO
}

bool Gateway::GetSimulationMode(void) const
{
    // TODO

    return false;
}

void Gateway::AttachPort(std::shared_ptr<types::Port> port)
{
}

types::Error Gateway::SetSuggestedSpeedAndAuthority(const types::TrackId track, const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority)
{
    // TODO serialize data, lookup wayside corresponding to track and block, send data
}

types::Error Gateway::SetMaintenanceMode(const types::TrackId track, const types::BlockId block, const bool maintenance)
{
}

types::Error Gateway::SetSwitchState(const types::TrackId track, const types::BlockId block, const bool switch_state)
{
}

void Gateway::GetBlockStates(const types::TrackId track, std::vector<BlockState> &states)
{
    // TODO
}


} // namespace wayside_controller