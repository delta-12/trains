/*****************************************************************************
 * @file track_model.h
 *
 * @brief Implements the track model backend.
 *****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H
#define TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H

#include <memory>
#include <vector>

#include "train_model.h"
#include "types.h"

namespace track_model
{

class TrackModel
{
public:
    types::TrackId GetTrackId(void);
    types::Error NewTrainModel(std::shared_ptr<train_model::TrainModel> train_model);
    void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &train_models) const;
    void Update(void);
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

private:
    std::vector<std::shared_ptr<train_model::TrainModel>> train_models; // TODO make iterable linked list, mostly going to be pushing and popping
};

} // namespace track_model

#endif // TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H