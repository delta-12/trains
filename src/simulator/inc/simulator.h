/*****************************************************************************
 * @file simulator.h
 *
 * @brief Handles simulating the physical world.
 *****************************************************************************/
#ifndef TRAINS_SRC_MODULES_SIMULATOR_INC_SIMULATOR_H
#define TRAINS_SRC_MODULES_SIMULATOR_INC_SIMULATOR_H

#include <memory>
#include <vector>

#include "track_model.h"
#include "train_model.h"
#include "types.h"

namespace simulator
{

class Simulator
{
public:
    types::Error AddTrackModel(std::shared_ptr<track_model::TrackModel>);
    types::Error DeleteTrackModel(const types::TrackId track);
    void GetTrackModels(std::vector<std::shared_ptr<track_model::TrackModel>> &track_models) const;
    types::Error AddTrainModel(const types::TrackId track, std::shared_ptr<train_model::TrainModel> train_model);
    void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &train_models) const;
    void Update(void);
private:
    std::vector<std::shared_ptr<track_model::TrackModel>> track_models;
};

} // namespace simulator

#endif // TRAINS_SRC_MODULES_SIMULATOR_INC_SIMULATOR_H