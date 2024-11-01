#include "train_model.h"
#include "tick_source.h"
#include "types.h"

#include <random>
#include <iostream>
#include <cstdint>
#include <chrono>

namespace train_model
{
SoftwareTrainModel::SoftwareTrainModel(std::shared_ptr<TickSource> clk) : clock_(clk)
{
        passengers_on_board_ = 0;
        headlights_ = 0;
        interior_light_ = 0;
        right_door_ = 0;
        left_door_ = 0;
        //track_pol;//must have starting polarity
        maximum_force_ = 120000;//maximum force of the engine
        previous_acceleration_ = 0;
        acceleration_ = 0;
        maximum_deceleration_limit_ = -2.73;//meters per second ^2
        maximum_acceleration_limit_ = .5;//meters per second ^2
        velocity_ = 0;
        previous_velocity_ = 0;
        maximum_velocity_ = 43.496;//mph
        maximum_passengers_ = 222;
        passengers_boarding_ = 0;
        crew_count_ = 9;//NEED TO CHECK HOW MANY THERE ARE
        train_mass_ = 37103.856;//kgs
        mass_ = 37103.856;
        force_ = 0;
        grade_ = 0;//can get this from moaz or just have it
        emergency_brake_ = 0;
        brake_failure_ = 0;
        engine_failure_ = 0;
        signal_pickup_failure_ = 0;
        beacon_data_;//beacon data needs instantiation
        authority_ = 0;
        commanded_speed_ = 0;
        train_id_ = 0;
        distance_traveled_ = 0;
        internal_temperature_ = 0;
        station_announcement_ = "Steel Plaza Station";
        service_brake_ = 0.0;
        power_ = 0;
        last_tick_updated_ = (*clock_).GetTick();
}
        void SoftwareTrainModel::Update()
        {
            std::chrono::milliseconds elapsed_time_ = (*clock_).GetElapsedTime(last_tick_updated_);

            types::Second delta = std::chrono::duration_cast<types::Second> (elapsed_time_);

            //internal calculations
            mass_ = train_mass_ + ((GetPassengersCount() + crew_count_) * 68.039);
            SpeedCalc(delta);

        }
        void SoftwareTrainModel::SetTrainId(const types::TrainId train)
        {
            train_id_ = train;
        }
        types::TrainId SoftwareTrainModel::GetTrainId(void) const
        {
            return train_id_;
        }
        void SoftwareTrainModel::SetEmergencyBrake(const bool emergency_brake)
        {
            emergency_brake_ = emergency_brake;
        }
        void SoftwareTrainModel::SetEngineFailure(const bool engine_failure)
        {
            engine_failure_ = engine_failure;
        }
        void SoftwareTrainModel::SetBrakeFailure(const bool brake_failure)
        {
            brake_failure_ = brake_failure;
        }
        void SoftwareTrainModel::SetSignalPickupFailure(const bool signal_pickup_failure)
        {
            signal_pickup_failure_ = signal_pickup_failure;
        }
        bool SoftwareTrainModel::GetBrakeFailure() const
        {
            return brake_failure_;
        }
        bool SoftwareTrainModel::GetEngineFailure() const
        {
            return engine_failure_;
        }
        bool SoftwareTrainModel::GetSignalPickupFailure() const
        {
            return signal_pickup_failure_;
        }
        //types::BeaconData SoftwareTrainModel::GetBeaconData(void) const
        //{
        //    return beacon_data_;
        //}
        types::Blocks SoftwareTrainModel::GetAuthority(void) const
        {
            return authority_;
        }
        void SoftwareTrainModel::SetCommandedSpeed(const types::MetersPerSecond speed)
        {
            commanded_speed_ = speed;
        }
        types::MetersPerSecond SoftwareTrainModel::GetCommandedSpeed(void) const
        {
            return commanded_speed_;
        }
        types::MetersPerSecond SoftwareTrainModel::GetActualSpeed(void) const
        {
            return velocity_;
        }
        types::Watts SoftwareTrainModel::GetActualPower(void) const
        {
            return power_;
        }
        void SoftwareTrainModel::SetCommandedPower(const types::Watts watts)
        {
            power_ = watts;
        }
        types::DegreesFahrenheit SoftwareTrainModel::GetActualInternalTemperature(void) const
        {
            return internal_temperature_;
        }
        types::Polarity SoftwareTrainModel::GetTrackPolarity(void) const
        {
            return track_polarity_;
        }
        void SoftwareTrainModel::SetStationAnnouncement(const std::string &announcement)
        {
            station_announcement_ = announcement;
        }
        void SoftwareTrainModel::SetGrade(const double grade)
        {
            grade_ = grade;
        }
        void SoftwareTrainModel::SetBrake(const double brake)
        {
            service_brake_ = brake;
        }
        void SoftwareTrainModel::SetHeadlights(const bool on)
        {
            headlights_ = on;
        }
        void SoftwareTrainModel::SetInternalLights(const bool on)
        {
            interior_light_ = on;
        }
        void SoftwareTrainModel::SetLeftDoorsState(const bool open)
        {
            left_door_ = open;
        }
        void SoftwareTrainModel::SetRightDoorsState(const bool open)
        {
            right_door_ = open;
        }
        void SoftwareTrainModel::SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees)
        {
            internal_temperature_ = degrees;
        }
        types::Meters SoftwareTrainModel::GetDistanceTraveled(void) const
        {
            return distance_traveled_;
        }
        void SoftwareTrainModel::SetDistanceTraveled(const types::Meters distance)
        {
            distance_traveled_ = distance;
        }
        uint16_t SoftwareTrainModel::GetPassengersDeboarding(void) const
        {
            //generate random number within bounds for leaving
            std::random_device              rd;              // Seed
            std::mt19937                    gen(rd());       // Mersenne Twister engine
            std::uniform_int_distribution<> dis(0, passengers_on_board_); // Uniform distribution between 0 and board

            // Generate a random number for passengers leaving
            uint16_t randomNumber = dis(gen);

            return randomNumber;
        }
        void SoftwareTrainModel::SetPassengersBoarding(const uint16_t passengers)
        {
            passengers_boarding_ = passengers;
        }
        types::Meters SoftwareTrainModel::GetDistanceTravelled(void) const
        {
            return distance_travelled_;
        }
        uint16_t SoftwareTrainModel::GetPassengersCount(void)
        {
            //adds new passengers
            passengers_on_board_ += passengers_boarding_;
            //removes old passengers
            passengers_on_board_ -= GetPassengersDeboarding();
            //returns passengers on board (should be called and updated only when at station)
            return passengers_on_board_;
        }
        void SoftwareTrainModel::SetAuthority(const types::Blocks blocks)
        {
            authority_ = blocks;
        }
        void SoftwareTrainModel::SetTrackPolarity(const types::Polarity polarity)
        {
            track_polarity_ = polarity;
        }
        //void SoftwareTrainModel::SetBeaconData(const types::BeaconData &data, std::size_t &size)
        //{
        //    beacon_data_ = data;
        //}
        void SoftwareTrainModel::SpeedCalc(types::Second delta)
        {
            if (velocity_ == 0 && power_ != 0)//avoids dividing by 0
            {
                force_ = maximum_force_;
            }
            else{
                force_ = power_/velocity_;
            }

            if (emergency_brake_ == true)
            {
                acceleration_ = -2.73;
            }
            else if (service_brake_ != 0)
            {
                acceleration_ = (-1.2 * service_brake_);
            }
            else
            {
            acceleration_ = force_/mass_;
            }
            previous_acceleration_ = acceleration_;
        
            previous_velocity_ = velocity_;
            velocity_ = previous_velocity_ + ((delta.count()/2) * (previous_acceleration_ + acceleration_));
        }
}