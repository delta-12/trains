/*****************************************************************************
 * @file wayside_controller_gateway.h
 *
 * @brief Implements communication between the CTC and Wayside Controllers .
 *****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_GATEWAY_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_GATEWAY_H

#include <memory>
#include <vector>

#include "track_model.h"
#include "types.h"

namespace wayside_controller
{

    typedef struct BlockOccupancy BlockOccupancy;

    struct BlockOccupancy
    {
    public:
        BlockOccupancy(const types::BlockId block, const bool occupied);
        types::BlockId block;
        bool occupied;
    };

    class Gateway
    {
    public:
        void UpdateControllers(void);
        types::Error UpdateTrackModel(std::shared_ptr<track_model::TrackModel> track_model);
        void SetSimulationMode(const bool simulation);
        bool GetSimulationMode(void) const;
        void AttachPort(std::shared_ptr<types::Port> port);
        types::Error SetSuggestedSpeed(const types::TrackId track, const types::BlockId block, const types::MetersPerSecond speed);
        types::Error SetAuthority(const types::TrackId track, const types::BlockId block, const types::Meters authority);
        types::Error SetMaintenanceMode(const types::TrackId track, const types::BlockId block, const bool maintenance);
        types::Error SetSwitchState(const types::TrackId track, const types::BlockId block, const bool switch_state);
        void GetBlockOccupancies(const types::TrackId track, std::vector<BlockOccupancy> &occupancies);
    };

} // namespace wayside_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_GATEWAY_H