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

namespace train_model
{

class TrainModel
{
    public:
        virtual void SetTrainId(const types::TrainId train)                                  = 0;
        virtual types::TrainId GetTrainId(void) const                                        = 0;
        virtual void SetEmergencyBrake(const bool emergency_brake)                           = 0;
        virtual void SetEngineFailure(const bool engine_failure)                             = 0;
        virtual void SetBrakeFailure(const bool brake_failure)                               = 0;
        virtual void SetSignalPickupFailure(const bool signal_pickup_failure)                = 0;
        virtual bool GetBrakeFailure(void) const                                             = 0;
        virtual bool GetEngineFailure(void) const                                            = 0;
        virtual bool GetSignalPickupFailure(void) const                                      = 0;
        virtual std::size_t GetBeaconData(types::BeaconData &data) const                     = 0;
        virtual types::Blocks GetAuthority(void) const                                       = 0;
        virtual void SetCommandedSpeed(types::MilesPerHour speed)                            = 0;
        virtual types::MilesPerHour GetCommandedSpeed(void) const                            = 0;
        virtual types::MetersPerSecond GetActualSpeed(void) const                            = 0;
        virtual types::Watts GetActualPower(void) const                                      = 0;
        virtual void SetActualPower(types::Watts watts)                                      = 0;
        virtual types::DegreesFahrenheit GetActualInternalTemperature(void) const            = 0;
        virtual types::Polarity GetTrackPolarity(void) const                                 = 0;
        virtual void SetGrade(const float grade)                                             = 0;
        virtual void SetBrake(const double brake)                                            = 0;
        virtual void SetStationAnnouncement(std::string announcement)                        = 0;
        virtual void SetHeadlights(const bool on)                                            = 0;
        virtual void SetInternalLights(const bool on)                                        = 0;
        virtual void SetLeftDoorsState(const bool open)                                      = 0;
        virtual void SetRightDoorsState(const bool open)                                     = 0;
        virtual void SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees) = 0;
        virtual types::Meters GetDistanceTraveled(void) const                                = 0;
        virtual void SetDistanceTraveled(const types::Meters distance)                       = 0;
        virtual uint16_t GetPassengersLeaving(void) const                                    = 0;
        virtual uint16_t GetPassengersCount(void)                                            = 0;
        virtual void SetCommandedSpeed(const types::MilesPerHour speed)                      = 0;
        virtual void SetAuthority(const types::Blocks blocks)                                = 0;
        virtual void SetPassengersBoarding(const uint16_t passengers)                        = 0;
        virtual void SetTrackPolarity(const types::Polarity polarity)                        = 0;
        virtual void SetBeaconData(const types::BeaconData &data, std::size_t &size)         = 0;

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
        float clk_frequency;
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
};

} // namespace train_model

#endif // TRAINS_SRC_MODULES_TRAIN_MODEL_INC_TRAIN_MODEL_H