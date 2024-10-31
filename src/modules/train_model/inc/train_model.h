/*****************************************************************************
* @file train_model.h
*
* @brief Declares the train model backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRAIN_MODEL_INC_TRAIN_MODEL_H
#define TRAINS_SRC_MODULES_TRAIN_MODEL_INC_TRAIN_MODEL_H

#include <cstdint>
#include <string>

#include "types.h"
#include "tick_source.h"

namespace train_model
{

class TrainModel
{
    public:
        TrainModel(std::shared_ptr<TickSource> clk);

        //internal
        void Update();
        void SpeedCalc(float delta_time);
        uint16_t GetPassengersLeaving(void) const;

        //setters
        void SetTrainId(const types::TrainId train);
        void SetEmergencyBrake(const bool emergency_brake);
        void SetEngineFailure(const bool engine_failure);
        void SetBrakeFailure(const bool brake_failure);
        void SetSignalPickupFailure(const bool signal_pickup_failure);
        void SetGrade(const float grade);
        void SetBrake(const double brake);
        void SetStationAnnouncement(std::string announcement);
        void SetHeadlights(const bool on);
        void SetInternalLights(const bool on);
        void SetLeftDoorsState(const bool open);
        void SetRightDoorsState(const bool open);
        void SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees);
        void SetCommandedSpeed(const types::MilesPerHour speed);
        void SetAuthority(const types::Blocks blocks);
        void SetPassengersBoarding(const uint16_t passengers);
        void SetTrackPolarity(const types::Polarity polarity);
        void SetBeaconData(const types::BeaconData &data, std::size_t &size);
        void SetDistanceTraveled(const types::Meters distance);
        void SetActualPower(const types::Watts watts);
        
        //getters
        types::TrainId GetTrainId(void) const;
        bool GetBrakeFailure(void) const;
        bool GetEngineFailure(void) const;
        bool GetSignalPickupFailure(void) const;
        std::size_t GetBeaconData(types::BeaconData &data) const;
        types::Blocks GetAuthority(void) const;
        types::MilesPerHour GetCommandedSpeed(void) const;
        types::MetersPerSecond GetActualSpeed(void) const;
        types::Watts GetActualPower(void) const;
        types::DegreesFahrenheit GetActualInternalTemperature(void) const;
        types::Polarity GetTrackPolarity(void) const;
        types::Meters GetDistanceTraveled(void) const;
        uint16_t GetPassengersCount(void);

        private:
        //internal variables(model specific)
        int passengers_on_board;
        bool ext_light;
        bool int_light;
        bool right_door;
        bool left_door;
        types::Polarity track_pol;
        //internal variables(calculations)
        float max_force = 120000;//maximum force of the engine
        types::MetersPerSecondSquared prev_acceleration = 0;
        types::MetersPerSecondSquared acceleration = 0;
        types::MetersPerSecondSquared max_dec_limit = -2.73;//meters per second ^2
        types::MetersPerSecondSquared max_acc_limit = .5;//meters per second ^2
        types::MilesPerHour velocity = 0;
        types::MilesPerHour prev_velocity = 0;
        types::MilesPerHour max_vel = 43.496;//mph
        types::MilesPerHour comm_speed;
        int max_passengers = 222;
        int passengers_boarding;
        int crew_count;
        float train_mass = 81800;//lbs
        float mass;
        float force;
        float grade;//can get this from moaz or just have it
        bool emergency_brake;
        bool brake_failure;
        bool engine_failure;
        bool signal_pickup_failure;
        types::BeaconData beacon_data;
        types::Blocks authority;
        types::TrainId train_id;
        types::Meters distance_traveled;
        types::DegreesFahrenheit int_temp;
        std::string station_announcement;
        double service_brake;
        types::Watts power;
        std::shared_ptr<TickSource> CLK;
};

} // namespace train_model

#endif // TRAINS_SRC_MODULES_TRAIN_MODEL_INC_TRAIN_MODEL_H