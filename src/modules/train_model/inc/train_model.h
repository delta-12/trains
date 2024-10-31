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
        TrainModel();
        TrainModel(std::shared_ptr<TickSource> clk);
        virtual void Update();
        virtual void SetTrainId(const types::TrainId train);
        virtual types::TrainId GetTrainId(void) const;
        virtual void SetEmergencyBrake(const bool emergency_brake);
        virtual void SetEngineFailure(const bool engine_failure);
        virtual void SetBrakeFailure(const bool brake_failure);
        virtual void SetSignalPickupFailure(const bool signal_pickup_failure);
        virtual bool GetBrakeFailure(void) const;
        virtual bool GetEngineFailure(void) const;
        virtual bool GetSignalPickupFailure(void) const;
        virtual std::size_t GetBeaconData(types::BeaconData &data) const;
        virtual types::Blocks GetAuthority(void) const;
        virtual void SetCommandedSpeed(types::MilesPerHour speed);
        virtual types::MilesPerHour GetCommandedSpeed(void) const;
        virtual types::MetersPerSecond GetActualSpeed(void) const;
        virtual types::Watts GetActualPower(void) const;
        virtual void SetActualPower(types::Watts watts);
        virtual types::DegreesFahrenheit GetActualInternalTemperature(void) const;
        virtual types::Polarity GetTrackPolarity(void) const;
        virtual void SetGrade(const float grade);
        virtual void SetBrake(const double brake);
        virtual void SetStationAnnouncement(std::string announcement);
        virtual void SetHeadlights(const bool on);
        virtual void SetInternalLights(const bool on);
        virtual void SetLeftDoorsState(const bool open);
        virtual void SetRightDoorsState(const bool open);
        virtual void SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees);
        virtual types::Meters GetDistanceTraveled(void) const;
        virtual void SetDistanceTraveled(const types::Meters distance);
        virtual uint16_t GetPassengersLeaving(void) const;
        virtual uint16_t GetPassengersCount(void);
        virtual void SetCommandedSpeed(const types::MilesPerHour speed);
        virtual void SetAuthority(const types::Blocks blocks);
        virtual void SetPassengersBoarding(const uint16_t passengers);
        virtual void SetTrackPolarity(const types::Polarity polarity);
        virtual void SetBeaconData(const types::BeaconData &data, std::size_t &size);
        virtual void SpeedCalc(float delta_time);

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
        types::MilesPerHour comm_speed;
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