#include "train_model.h"
#include "tick_source.h"
#include "types.h"

#include <random>
#include <iostream>
#include <cstdint>
#include <chrono>

namespace train_model
{
TrainModel::TrainModel(std::shared_ptr<TickSource> clk) : CLK(clk)
{
        passengers_on_board  = 0;
        ext_light = 0;
        int_light = 0;
        right_door = 0;
        left_door = 0;
        track_pol;//must have starting polarity
        max_force = 120000;//maximum force of the engine
        prev_acceleration = 0;
        acceleration = 0;
        max_dec_limit = -2.73;//meters per second ^2
        max_acc_limit = .5;//meters per second ^2
        velocity = 0;
        prev_velocity = 0;
        max_vel = 43.496;//mph
        max_passengers = 222;
        passengers_boarding = 0;
        crew_count = 9;//NEED TO CHECK HOW MANY THERE ARE
        train_mass = 37103.856;//kgs
        mass = 37103.856;
        force = 0;
        grade = 0;//can get this from moaz or just have it
        emergency_brake = 0;
        brake_failure = 0;
        engine_failure = 0;
        signal_pickup_failure = 0;
        beacon_data;
        authority = 0;
        comm_speed = 0;
        train_id = 0;
        distance_traveled = 0;
        int_temp = 0;
        station_announcement = "Steel Plaza Station";
        service_brake = 0.0;
        power = 0;
}
        void TrainModel::Update()
        {
            auto last_time = (*CLK).GetTick();

            auto delta_time_in_seconds = std::chrono::duration_cast<std::chrono::seconds> ((*CLK).GetElapsedTime(last_time));

            float delta_time = static_cast<float>(delta_time_in_seconds.count());

            mass = train_mass + ((TrainModel::GetPassengersCount() + crew_count) * 68.039);
            TrainModel::SpeedCalc(delta_time);

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
        void TrainModel::SetCommandedSpeed(const types::MilesPerHour speed)
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
        void TrainModel::SetActualPower(const types::Watts watts)
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
        void TrainModel::SpeedCalc(float delta_time)
        {
            if (velocity == 0 && power != 0)//avoids dividing by 0
            {
                TrainModel::force = TrainModel::max_force;
            }
            else{
                TrainModel::force = TrainModel::power/TrainModel::velocity;
            }

            if (TrainModel::emergency_brake == true)
            {
                acceleration = -2.73;
            }
            else if (TrainModel::service_brake != 0)
            {
                acceleration = (-1.2 * TrainModel::service_brake);
            }
            else
            {
            TrainModel::acceleration = force/mass;
            }
            TrainModel::prev_acceleration = acceleration;
        
            prev_velocity = velocity;
            velocity = prev_velocity + (delta_time/2) * (prev_acceleration + acceleration);
        }
}