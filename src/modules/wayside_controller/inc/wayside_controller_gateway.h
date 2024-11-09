/*****************************************************************************
* @file wayside_controller_gateway.h
*
* @brief Implements communication between the CTC and Wayside Controllers .
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_GATEWAY_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_GATEWAY_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "track_model.h"
#include "types.h"
#include "wayside_controller.h"

namespace wayside_controller
{

class Gateway
{
    public:
        void UpdateControllers(void);
        types::Error UpdateTrackModel(std::shared_ptr<track_model::TrackModel> track);
        types::Error UpdateTrackModels(std::vector<std::shared_ptr<track_model::TrackModel>> &tracks);
        void SetSimulationMode(const bool simulation);
        bool GetSimulationMode(void) const;
        void AttachPort(std::shared_ptr<types::Port> port);
        types::Error SetSuggestedSpeedAndAuthority(const types::TrackId track, const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority);
        types::Error SetMaintenanceMode(const types::TrackId track, const types::BlockId block, const bool maintenance);
        types::Error SetSwitchState(const types::TrackId track, const types::BlockId block, const bool switch_state);
        void GetBlockStates(const types::TrackId track, std::vector<types::BlockState> &states);

    private:
        std::vector<types::BlockState> block_states_;

};

} // namespace wayside_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_GATEWAY_H