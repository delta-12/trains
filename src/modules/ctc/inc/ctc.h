/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_H

#include <unordered_map>

#include "types.h"
#include "wayside_controller.h"
#include "wayside_controller_gateway.h"

namespace ctc
{

typedef struct
{
    types::BlockId block;
    types::Tick arrival_time;
} DestinationAndArrivalTime;

class Ctc
{
    public:
        void SetTrackLayout(const std::vector<types::Block> &blocks);
        void SetSchedule(const types::TrainId train, const std::vector<DestinationAndArrivalTime> &schedule);
        void SetManualMode(void);
        types::Error SetBlockStates(const types::TrackId track, const std::vector<wayside_controller::BlockState> &block_states);
        std::vector<wayside_controller::TrackCircuitData> GetSuggestedSpeedsAndAuthorities(void) const;
};

std::vector<std::string> SplitBySemicolon(const std::string& input);

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_H