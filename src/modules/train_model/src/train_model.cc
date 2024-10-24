#include "train_model.h"
#include <random>
#include <iostream>

namespace train_model
{
train_model::TrainModel::TrainModel()
{

}
        void TrainModel::SetTrainId(const types::TrainId train)
        {
            TrainModel::train_id = train;
        }
        types::TrainId TrainModel::GetTrainId(void) const
        {
            return TrainModel::train_id;
        }
        void TrainModel::SetEmergencyBrake(const bool emergency_brake)
        {
            TrainModel::emergency_brake = emergency_brake;
        }
        void TrainModel::SetEngineFailure(const bool engine_failure)
        {
            TrainModel::engine_failure = engine_failure;
        }
        void TrainModel::SetBrakeFailure(const bool brake_failure)
        {
            TrainModel::brake_failure = brake_failure;
        }
        void TrainModel::SetSignalPickupFailure(const bool signal_pickup_failure)
        {
            TrainModel::signal_pickup_failure = signal_pickup_failure;
        }
        bool TrainModel::GetBrakeFailure() const
        {
            return brake_failure;
        }
        bool TrainModel::GetEngineFailure() const
        {
            return engine_failure;
        }
        bool TrainModel::GetSignalPickupFailure() const
        {
            return signal_pickup_failure;
        }
        std::size_t TrainModel::GetBeaconData(types::BeaconData &data) const
        {
            //IDK WHAT TO DO HERE
        }
        types::Blocks TrainModel::GetAuthority(void) const
        {
            return TrainModel::authority;
        }
        void TrainModel::SetCommandedSpeed(types::MilesPerHour speed)
        {
            TrainModel::comm_speed = speed;
        }
        types::MilesPerHour TrainModel::GetCommandedSpeed(void) const
        {
            return TrainModel::comm_speed;
        }
        types::MetersPerSecond TrainModel::GetActualSpeed(void) const
        {
            return TrainModel::velocity;
        }
        bool TrainModel::GetEngineFailure(void) const
        {
            return TrainModel::engine_failure;
        }
        bool TrainModel::GetSignalPickupFailure(void) const
        {
            return TrainModel::signal_pickup_failure;
        }
        types::Watts TrainModel::GetActualPower(void) const
        {
            return TrainModel::power;
        }
        void TrainModel::SetActualPower(types::Watts watts)
        {
            TrainModel::power = watts;
        }
        types::DegreesFahrenheit TrainModel::GetActualInternalTemperature(void) const
        {
            return TrainModel::int_temp;
        }
        types::Polarity TrainModel::GetTrackPolarity(void) const
        {
            return TrainModel::track_pol;
        }
        void TrainModel::SetStationAnnouncement(const std::string announcement)
        {
            TrainModel::station_announcement = announcement;
        }
        void TrainModel::SetGrade(const float grade)
        {
            TrainModel::grade = grade;
        }
        void TrainModel::SetBrake(const double brake)
        {
            TrainModel::service_brake = brake;
        }
        void TrainModel::SetHeadlights(const bool on)
        {
            TrainModel::ext_light = on;
        }
        void TrainModel::SetInternalLights(const bool on)
        {
            TrainModel::int_light = on;
        }
        void TrainModel::SetLeftDoorsState(const bool open)
        {
            TrainModel::left_door = open;
        }
        void TrainModel::SetRightDoorsState(const bool open)
        {
            TrainModel::right_door = open;
        }
        void TrainModel::SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees)
        {
            TrainModel::int_temp = degrees;
        }
        types::Meters TrainModel::GetDistanceTraveled(void) const
        {
            return TrainModel::distance_traveled;
        }
        void TrainModel::SetDistanceTraveled(const types::Meters distance)
        {
            TrainModel::distance_traveled = distance;
        }
        uint16_t TrainModel::GetPassengersLeaving(void) const
        {
            //generate random number within bounds for leaving
            std::random_device              rd;              // Seed
            std::mt19937                    gen(rd());       // Mersenne Twister engine
            std::uniform_int_distribution<> dis(0, passengers_on_board); // Uniform distribution between 0 and board

            // Generate a random number for passengers leaving
            uint16_t randomNumber = dis(gen);

            return randomNumber;
        }
        void TrainModel::SetPassengersBoarding(const uint16_t passengers)
        {
            TrainModel::passengers_boarding = passengers;
        }
        uint16_t TrainModel::GetPassengersCount(void)
        {
            //adds new passengers
            TrainModel::passengers_on_board += passengers_boarding;
            //removes old passengers
            TrainModel::passengers_on_board -= TrainModel::GetPassengersLeaving();
            //returns passengers on board (should be called and updated only when at station)
            return TrainModel::passengers_on_board;
        }
        void TrainModel::SetCommandedSpeed(const types::MetersPerSecond speed)
        {
            TrainModel::comm_speed = speed;
        }
        void TrainModel::SetAuthority(const types::Blocks blocks)
        {
            TrainModel::authority = blocks;
        }
        void TrainModel::SetTrackPolarity(const types::Polarity polarity)
        {
            TrainModel::track_pol = polarity;
        }
        void TrainModel::SetBeaconData(const types::BeaconData &data, std::size_t &size)
        {
            TrainModel::beacon_data = data;
        }
}