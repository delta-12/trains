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
        virtual void SetDistanceTraveled(const types::Meters distance)                       = 0;
        virtual uint16_t GetPassengersDeboarding(void)                                       = 0;
        virtual types::Meters GetDistanceTraveled(void)                                      = 0;
        virtual void SetCommandedSpeed(const types::MetersPerSecond speed)                   = 0;
        virtual void SetAuthority(const types::Meters meters)                                = 0;
        virtual void SetPassengersBoarding(const uint16_t passegners)                        = 0;
        virtual void SetTrackPolarity(const types::Polarity polarity)                        = 0;
        virtual void SetBeaconData(const types::BeaconData &data, std::size_t &size)         = 0;
};

} // namespace train_model

#endif // TRAINS_SRC_MODULES_TRAIN_MODEL_INC_TRAIN_MODEL_H