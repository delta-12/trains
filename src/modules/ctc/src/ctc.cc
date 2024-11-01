/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CC
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CC

#include "ctc.h"

namespace ctc
{

void Ctc::SetTrackLayout(const std::vector<types::Block> &blocks)
{
}

void Ctc::SetSchedule(const types::TrainId train, const std::vector<DestinationAndArrivalTime> &schedule)
{
}

void Ctc::SetManualMode(void)
{
}

types::Error Ctc::SetBlockStates(const types::TrackId track, const std::vector<types::BlockState> &block_states)
{
    return types::ERROR_INVALID_TRACK;
}

std::vector<types::TrackCircuitData> Ctc::GetSuggestedSpeedsAndAuthorities(void) const
{
    return std::vector<types::TrackCircuitData>();
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC