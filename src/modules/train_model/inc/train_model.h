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
        //setters
        virtual void SetTrainId(const types::TrainId train) = 0;
        virtual void SetEmergencyBrake(const bool emergency_brake) = 0;
        virtual void SetEngineFailure(const bool engine_failure) = 0;
        virtual void SetBrakeFailure(const bool brake_failure) = 0;
        virtual void SetSignalPickupFailure(const bool signal_pickup_failure) = 0;
        virtual void SetGrade(const float grade) = 0;
        virtual void SetBrake(const double brake);
        virtual void SetStationAnnouncement(std::string announcement) = 0;
        virtual void SetHeadlights(const bool on) = 0;
        virtual void SetInternalLights(const bool on) = 0;
        virtual void SetLeftDoorsState(const bool open) = 0;
        virtual void SetRightDoorsState(const bool open) = 0;
        virtual void SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees) = 0;
        virtual void SetCommandedSpeed(const types::MilesPerHour speed) = 0;
        virtual void SetAuthority(const types::Blocks blocks) = 0;
        virtual void SetPassengersBoarding(const uint16_t passengers) = 0;
        virtual void SetTrackPolarity(const types::Polarity polarity) = 0;
        virtual void SetBeaconData(const types::BeaconData &data, std::size_t &size) = 0;
        virtual void SetDistanceTraveled(const types::Meters distance) = 0;
        virtual void SetActualPower(const types::Watts watts) = 0;
        
        //getters
        virtual types::TrainId GetTrainId(void) const;
        virtual bool GetBrakeFailure(void) const;
        virtual bool GetEngineFailure(void) const;
        virtual bool GetSignalPickupFailure(void) const;
        virtual types::BeaconData GetBeaconData(void) const;
        virtual types::Blocks GetAuthority(void) const;
        virtual types::MilesPerHour GetCommandedSpeed(void) const;
        virtual types::MetersPerSecond GetActualSpeed(void) const;
        virtual types::Watts GetActualPower(void) const;
        virtual types::DegreesFahrenheit GetActualInternalTemperature(void) const;
        virtual types::Polarity GetTrackPolarity(void) const;
        virtual types::Meters GetDistanceTraveled(void) const;
        virtual uint16_t GetPassengersCount(void);
};


class SoftwareTrainModel : public TrainModel
{
    public:
        SoftwareTrainModel(std::shared_ptr<TickSource> clk);

        //internal
        void Update();
        void SpeedCalc(types::Second delta);
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
        types::BeaconData GetBeaconData(void) const;
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
        int passengers_on_board_;
        bool headlights_;
        bool interior_light_;
        bool right_door_;
        bool left_door_;
        types::Polarity track_polarity_;
        //internal variables(calculations)
        float maximum_force_;
        types::MetersPerSecondSquared previous_acceleration_;
        types::MetersPerSecondSquared acceleration_;
        types::MetersPerSecondSquared maximum_deceleration_limit_;
        types::MetersPerSecondSquared maximum_acceleration_limit_;
        types::MilesPerHour velocity_;
        types::MilesPerHour previous_velocity_;
        types::MilesPerHour maximum_velocity_;
        types::MilesPerHour commanded_speed_;
        int maximum_passengers_;
        int passengers_boarding_;
        int crew_count_;
        float train_mass_;
        float mass_;
        float force_;
        float grade_;//can get this from moaz or just have it
        bool emergency_brake_;
        bool brake_failure_;
        bool engine_failure_;
        bool signal_pickup_failure_;
        types::BeaconData beacon_data_;
        types::Blocks authority_;
        types::TrainId train_id_;
        types::Meters distance_traveled_;
        types::DegreesFahrenheit internal_temperature_;
        std::string station_announcement_;
        double service_brake_;
        types::Watts power_;
        //counter variables
        std::shared_ptr<TickSource> clock_;
        types::Tick last_tick_updated_;
};

} // namespace train_model

#endif // TRAINS_SRC_MODULES_TRAIN_MODEL_INC_TRAIN_MODEL_H