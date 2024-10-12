#include "wayside_controller_gateway.h"

namespace wayside_controller
{

void Gateway::UpdateControllers(void)
{
}

types::Error Gateway::UpdateTrackModel(std::shared_ptr<track_model::TrackModel> track)
{
    // TODO NNF-172 receive data
}

types::Error Gateway::UpdateTrackModels(std::vector<std::shared_ptr<track_model::TrackModel> > &tracks)
{
    // TODO NNF-172
}

void Gateway::SetSimulationMode(const bool simulation)
{
    // TODO NNF-172
}

bool Gateway::GetSimulationMode(void) const
{
    // TODO NNF-172

    return false;
}

void Gateway::AttachPort(std::shared_ptr<types::Port> port)
{
    // TODO NNF-172
}

types::Error Gateway::SetSuggestedSpeedAndAuthority(const types::TrackId track, const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority)
{
    // TODO NNF-172 serialize data, lookup wayside corresponding to track and block, send data
}

types::Error Gateway::SetMaintenanceMode(const types::TrackId track, const types::BlockId block, const bool maintenance)
{
    // TODO NNF-172
}

types::Error Gateway::SetSwitchState(const types::TrackId track, const types::BlockId block, const bool switch_state)
{
    // TODO NNF-172
}

void Gateway::GetBlockStates(const types::TrackId track, std::vector<BlockState> &states)
{
    // TODO NNF-172
}


} // namespace wayside_controller