/*****************************************************************************
* @file simulator.h
*
* @brief Handles simulating the physical world.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_SIMULATOR_INC_SIMULATOR_H
#define TRAINS_SRC_MODULES_SIMULATOR_INC_SIMULATOR_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "track_model.h"
#include "train_controller_gateway.h"
#include "train_model.h"
#include "types.h"
#include "wayside_controller_gateway.h"

namespace simulator
{

class Simulator
{
    public:
        types::Error AddTrackModel(std::shared_ptr<track_model::TrackModel> track);
        types::Error DeleteTrackModel(const types::TrackId track);
        void GetTrackModels(std::vector<std::shared_ptr<track_model::TrackModel>> &tracks) const;
        types::Error AddTrainModel(const types::TrackId track, std::shared_ptr<train_model::TrainModel> train);
        void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains) const;
        types::Error SetTrackCircuitData(const types::TrackCircuitData &data);
    // TODO add methods for each set communication between modules

    private:
        std::unordered_map<types::TrackId, std::shared_ptr<track_model::TrackModel>> tracks_;
};

} // namespace simulator

#endif // TRAINS_SRC_MODULES_SIMULATOR_INC_SIMULATOR_H