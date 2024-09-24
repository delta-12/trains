/*****************************************************************************
 * @file track_model.h
 *
 * @brief Implements the track model backend.
 *****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H
#define TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H

#include "types.h"

namespace track_model
{

class TrackModel
{
public:
    types::Error SetSwitchState(const types::BlockId block, const bool switched);
    types::Error SetCrossingState(const types::BlockId block, const bool closed);
    types::Error SetRedTrafficLight(const types::BlockId block, const bool on);
    types::Error SetYelloTrafficLight(const types::BlockId block, const bool on);
    types::Error SetGreenTrafficLight(const types::BlockId block, const bool on);
    types::Error SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed);
    types::Error SetAuthority(const types::BlockId block, const types::Meters authority);
    types::Error GetBlockOccupancy(const types::BlockId block, bool &occupied) const;
    types::Error SetBrokenRail(const types::BlockId block, const bool broken);
    types::Error SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure);
    types::Error SetPowerFailure(const types::BlockId block, const bool power_failure);
    types::Error GetCommandedSpeed(const types::TrainId train, types::MetersPerSecond &speed) const;
    types::Error GetAuthority(const types::TrainId train, types::Meters &authority) const;
    types::Error GetPassengersBoarding(const types::TrainId train, uint16_t &passengers) const;
    types::Error GetTrackPolarity(const types::TrainId train, types::Polarity &polarity) const;
    types::Error GetBeaconData(const types::TrainId train, types::BeaconData &data, std::size_t &size);
};

} // namespace track_model

#endif // TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H