#include "train_model.h"
#include "types.h"

namespace train_model
{

void TrainModelImpl::SetTrainId(const types::TrainId train)
{
    train_id_ = train;
}

types::TrainId TrainModelImpl::GetTrainId(void) const
{
    return train_id_;
}

void TrainModelImpl::SetEmergencyBrake(const bool emergency_brake)
{
    emergency_brake_ = emergency_brake;
}

void TrainModelImpl::SetEngineFailure(const bool engine_failure)
{
    engine_failure_ = engine_failure;
}

void TrainModelImpl::SetBrakeFailure(const bool brake_failure)
{
    brake_failure_ = brake_failure;
}

void TrainModelImpl::SetSignalPickupFailure(const bool signal_pickup_failure)
{
    signal_pickup_failure_ = signal_pickup_failure;
}

bool TrainModelImpl::GetBrakeFailure(void) const
{
    return brake_failure_;
}

std::size_t TrainModelImpl::GetBeaconData(types::BeaconData &data) const
{
    // Stub for getting beacon data, return size of data
    return sizeof(data);
}

types::Blocks TrainModelImpl::GetAuthority(void) const
{
    return authority_;
}

types::MetersPerSecond TrainModelImpl::GetCommandedSpeed(void) const
{
    return commanded_speed_;
}

types::MetersPerSecond TrainModelImpl::GetActualSpeed(void) const
{
    return actual_speed_;
}

bool TrainModelImpl::GetEngineFailure(void) const
{
    return engine_failure_;
}

bool TrainModelImpl::GetSignalPickupFailure(void) const
{
    return signal_pickup_failure_;
}

types::Watts TrainModelImpl::GetActualPower(void) const
{
    return actual_power_;
}

types::DegreesFahrenheit TrainModelImpl::GetActualInternalTemperature(void) const
{
    return internal_temperature_;
}

types::Polarity TrainModelImpl::GetTrackPolarity(void) const
{
    return track_polarity_;
}

void TrainModelImpl::SetStationAnnouncement(const std::string &announcement)
{
    // Handle station announcement
    announcement_ = announcement;
}

void TrainModelImpl::SetGrade(const float grade)
{
    grade_ = grade;
}

void TrainModelImpl::SetBrake(const float brake)
{
    // Handle brake logic
    brake_ = brake;
}

void TrainModelImpl::SetHeadlights(const bool on)
{
    headlights_on_ = on;
}

void TrainModelImpl::SetInternalLights(const bool on)
{
    internal_lights_on_ = on;
}

void TrainModelImpl::SetLeftDoorsState(const bool open)
{
    left_doors_open_ = open;
}

void TrainModelImpl::SetRightDoorsState(const bool open)
{
    right_doors_open_ = open;
}

void TrainModelImpl::SetCommandedPower(const types::Watts power)
{
    commanded_power_ = power;
}

void TrainModelImpl::SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees)
{
    internal_temperature_ = degrees;
}

void TrainModelImpl::SetBlockId(const types::BlockId block)
{
    block_id_ = block;
}

uint16_t TrainModelImpl::GetPassengersDeboarding(void)
{
    if (current_passengers_ == 0)
    {
        return 0;
    }
    else
    {
        return 5;
    }
}

types::Meters TrainModelImpl::GetDistanceTraveled(void)
{
    return 110;
}

void TrainModelImpl::SetCommandedSpeed(const types::MetersPerSecond speed)
{
    commanded_speed_ = speed;
}

void TrainModelImpl::SetAuthority(const types::Blocks blocks)
{
    authority_ = blocks;
}

void TrainModelImpl::SetPassengersBoarding(const uint16_t passengers)
{
    current_passengers_ += passengers;
}

void TrainModelImpl::SetTrackPolarity(const types::Polarity polarity)
{
    track_polarity_ = polarity;
}

void TrainModelImpl::SetBeaconData(const types::BeaconData &data, std::size_t &size)
{
    // Stub for setting beacon data
    size = sizeof(data);
}

} // namespace train_model

