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

#define TRAIN_MODEL_MAXIMUM_PASSENGERS 222
#define TRAIN_MODEL_LENGTH             32

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
        virtual std::size_t GetBeaconData(types::BeaconData &data) const                     = 0;
        virtual types::Meters GetAuthority(void) const                                       = 0;
        virtual types::MetersPerSecond GetCommandedSpeed(void) const                         = 0;
        virtual types::MetersPerSecond GetActualSpeed(void) const                            = 0;
        virtual bool GetEngineFailure(void) const                                            = 0;
        virtual bool GetSignalPickupFailure(void) const                                      = 0;
        virtual types::Watts GetActualPower(void) const                                      = 0;
        virtual types::DegreesFahrenheit GetActualInternalTemperature(void) const            = 0;
        virtual types::Polarity GetTrackPolarity(void) const                                 = 0;
        virtual void SetStationAnnouncement(const std::string &announcement)                 = 0;
        virtual void SetGrade(const float grade)                                             = 0;
        virtual void SetBrake(const float brake)                                             = 0;
        virtual void SetHeadlights(const bool on)                                            = 0;
        virtual void SetInternalLights(const bool on)                                        = 0;
        virtual void SetLeftDoorsState(const bool open)                                      = 0;
        virtual void SetRightDoorsState(const bool open)                                     = 0;
        virtual void SetCommandedPower(const types::Watts power)                             = 0;
        virtual void SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees) = 0;
        virtual void SetBlockId(const types::BlockId block)                                  = 0;
        virtual uint16_t GetPassengersDeboarding(void)                                       = 0;
        virtual types::Meters GetDistanceTraveled(void)                                      = 0;
        virtual void SetCommandedSpeed(const types::MetersPerSecond speed)                   = 0;
        virtual void SetAuthority(const types::Meters meters)                                = 0;
        virtual void SetPassengersBoarding(const uint16_t passegners)                        = 0;
        virtual void SetTrackPolarity(const types::Polarity polarity)                        = 0;
        virtual void SetBeaconData(const types::BeaconData &data, std::size_t &size)         = 0;
};

// Definition of the TrainModelImpl class
class TrainModelImpl : public TrainModel
{
    public:
        void SetTrainId(const types::TrainId train);
        types::TrainId GetTrainId(void) const;
        void SetEmergencyBrake(const bool emergency_brake);
        void SetEngineFailure(const bool engine_failure);
        void SetBrakeFailure(const bool brake_failure);
        void SetSignalPickupFailure(const bool signal_pickup_failure);
        bool GetBrakeFailure(void) const;
        std::size_t GetBeaconData(types::BeaconData &data) const;
        types::Meters GetAuthority(void) const;
        types::MetersPerSecond GetCommandedSpeed(void) const;
        types::MetersPerSecond GetActualSpeed(void) const;
        bool GetEngineFailure(void) const;
        bool GetSignalPickupFailure(void) const;
        types::Watts GetActualPower(void) const;
        types::DegreesFahrenheit GetActualInternalTemperature(void) const;
        types::Polarity GetTrackPolarity(void) const;
        void SetStationAnnouncement(const std::string &announcement);
        void SetGrade(const float grade);
        void SetBrake(const float brake);
        void SetHeadlights(const bool on);
        void SetInternalLights(const bool on);
        void SetLeftDoorsState(const bool open);
        void SetRightDoorsState(const bool open);
        void SetCommandedPower(const types::Watts power);
        void SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees);
        void SetBlockId(const types::BlockId block);
        uint16_t GetPassengersDeboarding(void);
        types::Meters GetDistanceTraveled(void);
        void SetCommandedSpeed(const types::MetersPerSecond speed);
        void SetAuthority(const types::Meters meters);
        void SetPassengersBoarding(const uint16_t passengers);
        void SetTrackPolarity(const types::Polarity polarity);
        void SetBeaconData(const types::BeaconData &data, std::size_t &size);

    private:
        types::TrainId train_id_;
        bool emergency_brake_;
        bool engine_failure_;
        bool brake_failure_;
        bool signal_pickup_failure_;
        types::Meters commanded_speed_;
        types::MetersPerSecond actual_speed_;
        types::Meters authority_;
        types::Meters distance_traveled_;
        uint16_t passengers_deboarding_;
        uint16_t current_passengers_ = 0;
        bool left_doors_open_;
        bool right_doors_open_;
        types::DegreesFahrenheit internal_temperature_;
        types::Watts commanded_power_;
        types::Watts actual_power_;
        types::Polarity track_polarity_;
        std::string announcement_;
        float grade_;
        float brake_;
        bool headlights_on_;
        bool internal_lights_on_;
        types::BlockId block_id_;
};

} // namespace train_model

#endif // TRAINS_SRC_MODULES_TRAIN_MODEL_INC_TRAIN_MODEL_H