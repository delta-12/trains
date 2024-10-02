/*****************************************************************************
* @file track_model.h
*
* @brief Declares the track model backend.
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
        /*virtual types::TrackId GetTrackId(void)                                                                   = 0;
        virtual types::Error AddTrainModel(std::shared_ptr<train_model::TrainModel> train)                        = 0;
        virtual void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel> > &trains) const         = 0;
        virtual void Update(void)                                                                                 = 0;
        virtual types::Error SetSwitchState(const types::BlockId block, const bool switched)                      = 0;
        virtual types::Error SetCrossingState(const types::BlockId block, const bool closed)                      = 0;
        virtual types::Error SetRedTrafficLight(const types::BlockId block, const bool on)                        = 0;
        virtual types::Error SetYelloTrafficLight(const types::BlockId block, const bool on)                      = 0;
        virtual types::Error SetGreenTrafficLight(const types::BlockId block, const bool on)                      = 0;
        virtual types::Error SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed)    = 0;
        virtual types::Error SetAuthority(const types::BlockId block, const types::Meters authority)              = 0;
        */virtual types::Error GetBlockOccupancy(const types::BlockId block, bool &occupied) const                  = 0;
        virtual types::Error SetBrokenRail(const types::BlockId block, const bool broken)                         = 0;
        virtual types::Error SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure) = 0;
        virtual types::Error SetPowerFailure(const types::BlockId block, const bool power_failure)                = 0;
};

class SoftwareTrackModel : public TrackModel {
    public:
        SoftwareTrackModel();
        /*virtual types::TrackId GetTrackId(void)                                                                   = 0;
        virtual types::Error AddTrainModel(std::shared_ptr<train_model::TrainModel> train)                        = 0;
        virtual void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel> > &trains) const         = 0;
        virtual void Update(void)                                                                                 = 0;
        virtual types::Error SetSwitchState(const types::BlockId block, const bool switched)                      = 0;
        virtual types::Error SetCrossingState(const types::BlockId block, const bool closed)                      = 0;
        virtual types::Error SetRedTrafficLight(const types::BlockId block, const bool on)                        = 0;
        virtual types::Error SetYelloTrafficLight(const types::BlockId block, const bool on)                      = 0;
        virtual types::Error SetGreenTrafficLight(const types::BlockId block, const bool on)                      = 0;
        virtual types::Error SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed)    = 0;
        virtual types::Error SetAuthority(const types::BlockId block, const types::Meters authority)              = 0;
        */virtual types::Error GetBlockOccupancy(const types::BlockId block, bool &occupied) const;
        virtual types::Error SetBrokenRail(const types::BlockId block, const bool broken);
        virtual types::Error SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure);
        virtual types::Error SetPowerFailure(const types::BlockId block, const bool power_failure);

    private:
        types::TrackId track_id;
        std::vector<std::shared_ptr<train_model::TrainModel>> trainsvec;
        int temp;

        struct Block
        {
            bool occupancy=0;
            int switches[2]={0, 0};
            bool crossing=0;
            bool light=0;
            bool pfail=0;
            bool tcfail=0;
            bool brail=0;
            int grade=0;
            int length=0;
            int slimit=50;
            int elevation=0;
            char section='A';
            bool heater=0;
            char station='Z';
        
        };

        std::vector<Block> line;

        int maxpass=222;

        //struct for the content, make a vector of them
}; //hi
} // namespace track_model

#endif // TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H