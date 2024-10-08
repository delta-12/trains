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
#include <iostream>
#include <string>

namespace track_model
{

class TrackModel
{
    public:
        virtual types::TrackId GetTrackId(void)                                                                   = 0;
        virtual types::Error AddTrainModel(std::shared_ptr<train_model::TrainModel> train)                        = 0;
        virtual void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel> > &trains) const         = 0;
        virtual void Update(void)                                                                                 = 0;
        virtual types::Error SetSwitchState(const types::BlockId block, const bool switched)                      = 0;
        virtual types::Error SetCrossingState(const types::BlockId block, const bool closed)                      = 0;
        virtual types::Error SetRedTrafficLight(const types::BlockId block, const bool on)                        = 0;
        virtual types::Error SetYelloTrafficLight(const types::BlockId block, const bool on)                      = 0;
        virtual types::Error SetGreenTrafficLight(const types::BlockId block, const bool on)                      = 0;
        virtual types::Error SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed)    = 0; //other modules
        virtual types::Error SetAuthority(const types::BlockId block, const types::Meters authority)              = 0; //other modules
        virtual types::Error GetBlockOccupancy(const types::BlockId block, bool &occupied) const                  = 0;
        virtual types::Error SetBrokenRail(const types::BlockId block, const bool broken)                         = 0;
        virtual types::Error SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure) = 0;
        virtual types::Error SetPowerFailure(const types::BlockId block, const bool power_failure)                = 0;
        virtual types::Error SetExternalTemperature(const types::DegreesFahrenheit temperature)                   = 0;
};

class SoftwareTrackModel : public TrackModel
{
    public:
        SoftwareTrackModel();
        SoftwareTrackModel(const std::vector<std::vector<std::string>> records);
        virtual types::TrackId GetTrackId(void);
        virtual types::Error AddTrainModel(std::shared_ptr<train_model::TrainModel> train);
        virtual void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel> > &trains) const;
        virtual void Update(void);
        virtual types::Error SetSwitchState(const types::BlockId block, const bool switched);
        virtual types::Error SetCrossingState(const types::BlockId block, const bool closed);
        virtual types::Error SetRedTrafficLight(const types::BlockId block, const bool on);
        virtual types::Error SetYelloTrafficLight(const types::BlockId block, const bool on);
        virtual types::Error SetGreenTrafficLight(const types::BlockId block, const bool on);
        virtual types::Error SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed);
        virtual types::Error SetAuthority(const types::BlockId block, const types::Meters authority);
        virtual types::Error GetBlockOccupancy(const types::BlockId block, bool &occupied) const;
        virtual types::Error SetBrokenRail(const types::BlockId block, const bool broken);
        virtual types::Error SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure);
        virtual types::Error SetPowerFailure(const types::BlockId block, const bool power_failure);
        virtual types::Error SetExternalTemperature(const types::DegreesFahrenheit temperature);

    private:
        types::TrackId track_id=1;
        std::vector<std::shared_ptr<train_model::TrainModel> > trainsvec;
        std::vector<std::vector<types::BlockId>> trainblockvec;
        std::vector<uint16_t> tpassengers;
        int externaltemp;

        struct Block
        {
            types::BlockId blocknum;
            bool occupancy = 0;
            types::BlockId Switch;
            bool crossing          = 0;
            bool light             = 0;
            std::string lightcolor = "GREEN";
            bool pfail             = 0;
            bool tcfail            = 0;
            bool brail             = 0;
            float grade              = 0;
            types::Meters length   = 50;
            int slimit             = 50;
            float elevation          = 0;
            std::string section           = "A";
            bool heater            = 0;
            char station           = 'Z';
            int direction          = 0;
            types::BlockId connection=0;
            bool underground       = 0;
            float cumelevation = 0;
            float sectotraverse;

        };

        //initial vector with all blocks loaded from the csv (think of this as a database)
        std::vector<Block> allblocks;

        //vector containing current actual line with swtiches and branches factored in
        std::vector<Block> line;

        int maxpass = 222;

        std::vector<std::shared_ptr<train_model::TrainModel> > trainmodels;

        types::Meters tlength = 32;

        std::string linetype = "blue";
};
} // namespace track_model

#endif // TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H