/*****************************************************************************
 * @file train_model.h
 *
 * @brief Implements the train model backend.
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
    TrainModel(const types::TrainId train);
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
    void SetDistanceTraveled(const types::Meters distance);
    uint16_t GetPassengersDeboarding(void);
    types::Meters GetDistanceTraveled(void);
    void SetCommandedSpeed(const types::MetersPerSecond speed);
    void SetAuthority(const types::Meters meters);
    void SetPassengersBoarding(const uint16_t passegners);
    void SetTrackPolarity(const types::Polarity polarity);
    void SetBeaconData(const types::BeaconData &data, std::size_t &size);
};

} // namespace train_model

#endif // TRAINS_SRC_MODULES_TRAIN_MODEL_INC_TRAIN_MODEL_H