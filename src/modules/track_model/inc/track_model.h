/*****************************************************************************
* @file track_model.h
*
* @brief Declares the track model backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H
#define TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "train_model.h"
#include "types.h"

namespace track_model
{

typedef enum
{
    LIGHTCOLOR_RED,
    LIGHTCOLOR_GREEN
} LightColor;

typedef enum
{
    BLOCKDIRECTION_UNIDIRECTIONAL,
    BLOCKDIRECTION_BIDIRECTIONAL
} BlockDirection;

typedef struct
{
    types::BlockId block;
    bool occupied;
    bool has_switch;
    bool switched;
    types::BlockId switch_connection;
    bool has_crossing;
    bool crossing_state;
    bool has_light;
    LightColor light_color;
    bool power_failure;
    bool track_circuit_failure;
    bool broken_rail;
    double grade;
    types::Meters length;
    types::MetersPerSecond speed_limit;
    types::Meters elevation;
    types::Meters cumulative_elevation;
    types::SectionId section;
    bool heater_on;
    bool has_station;
    BlockDirection direction;
    bool underground;
} Block;

class TrackModel
{
    public:
        virtual types::TrackId GetTrackId(void)                                                                = 0;
        virtual types::Error AddTrainModel(std::shared_ptr<train_model::TrainModel> train)                     = 0;
        virtual void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel> > &trains) const      = 0;
        virtual void Update(void)                                                                              = 0;
        virtual types::Error SetSwitchState(const types::BlockId block, const bool switched)                   = 0;
        virtual types::Error SetCrossingState(const types::BlockId block, const bool closed)                   = 0;
        virtual types::Error SetRedTrafficLight(const types::BlockId block, const bool on)                     = 0;
        virtual types::Error SetYellowTrafficLight(const types::BlockId block, const bool on)                  = 0;
        virtual types::Error SetGreenTrafficLight(const types::BlockId block, const bool on)                   = 0;
        virtual types::Error SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed) = 0;
        virtual types::Error SetAuthority(const types::BlockId block, const types::Meters authority)           = 0;
        virtual types::Error GetBlockOccupancy(const types::BlockId block, bool &occupied) const               = 0;
};

class SoftwareTrackModel : public TrackModel
{
    public:
        types::Error SetTrackLayout(const types::TrackId track, const std::vector<Block> &blocks);
        types::TrackId GetTrackId(void);
        types::Error AddTrainModel(std::shared_ptr<train_model::TrainModel> train);
        void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel> > &trains) const;
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
        types::Error SetExternalTemperature(const types::DegreesFahrenheit temperature);
        types::Error SetTrainBlock(const types::BlockId block);
        Block GetBlock(const types::BlockId block);

    private:
        types::TrackId track_;
        std::vector<Block> blocks_;
        std::vector<std::shared_ptr<train_model::TrainModel> > trains_;
        std::vector<types::BlockId> occupied_blocks_;
        std::vector<uint16_t> passenger_counts_;
        types::DegreesFahrenheit external_temperature_;
};

} // namespace track_model

#endif // TRAINS_SRC_MODULES_TRACK_MODEL_INC_TRACK_MODEL_H