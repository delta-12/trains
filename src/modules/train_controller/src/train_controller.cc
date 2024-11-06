#include "train_controller.h"

#include <unistd.h>
#include <cstdint>
#include <chrono>

#include "types.h"
#include "convert.h"
#include "tick_source.h"

namespace train_controller
{
//Constructor
SoftwareTrainController::SoftwareTrainController(std::shared_ptr<TickSource> clk) : clock_(clk)
{
    // Initializing variables
    ki_                             = TRAIN_CONTROLLER_DEFAULT_KI;
    kp_                             = TRAIN_CONTROLLER_DEFAULT_KP;
    max_power_                      = TRAIN_CONTROLLER_MAXIMUM_ENGINE_POWER;
    commanded_internal_temperature_ = DEFAULT_TRAIN_TEMPERATURE;
    train_max_speed_                = TRAIN_SPEED_LIMIT;
    set_route_position_             = 0;


    distance_of_authority_in_meters_     = 0;
    distance_since_last_update_          = 0;
    integral_sum_                        = 0;
    commanded_speed_                     = 0;
    driver_speed_                        = 0;
    current_speed_                       = 0;
    service_brake_percentage_            = 0;
    commanded_power_                     = 0;
    authority_                           = 0;
    emergency_brake_                     = 0;
    headlights_                          = 0;
    interior_lights_                     = 0;
    left_door_                           = 0;
    right_door_                          = 0;
    brake_failure_                       = 0;
    signal_pickup_failure_               = 0;
    engine_failure_                      = 0;
    actual_internal_temperature_         = 0;
    distance_travelled_                  = 0;
    distance_prior_to_current_authority_ = 0;
    total_blocks_accessed_length_        = (green_block_data_map_[green_default_route_vector_[set_route_position_]])[0];
    arrived_                             = 0;
    operation_mode_                      = false;
    last_tick_updated_                   = (*clock_).GetTick();

    polarity_          = types::Polarity::POLARITY_NEGATIVE;
    last_polarity_     = polarity_;
    usable_authority_  = authority_;
    authority_counter_ = authority_;
    new_authority_     = false;

    Update();
}


// Getters
double SoftwareTrainController::GetServiceBrake() const
{
    return service_brake_percentage_;
}

double SoftwareTrainController::GetGrade(void) const
{
    return DEFAULT_BLOCK_GRADE;
}

types::MilesPerHour SoftwareTrainController::GetDriverSpeed(void) const
{
    return convert::MetersPerSecondToMilesPerHour(driver_speed_);
}

types::Watts SoftwareTrainController::GetCommandedPower() const
{
    return commanded_power_;
}

types::Meters SoftwareTrainController::GetDistanceTravelled(void) const
{
    return distance_travelled_;
}

types::Meters SoftwareTrainController::GetDistanceTravelledSinceLastUpdate(void) const
{
    return distance_since_last_update_;
}

bool SoftwareTrainController::GetEmergencyBrake() const
{
    return emergency_brake_;
}

bool SoftwareTrainController::GetHeadLights() const
{
    return headlights_;
}

bool SoftwareTrainController::GetInteriorLights() const
{
    return interior_lights_;
}

bool SoftwareTrainController::GetLeftDoors() const
{
    return left_door_;
}

bool SoftwareTrainController::GetRightDoors() const
{
    return right_door_;
}

bool SoftwareTrainController::GetBrakeFailure() const
{
    return brake_failure_;
}
bool SoftwareTrainController::GetEngineFailure() const
{
    return engine_failure_;
}

bool SoftwareTrainController::GetSignalPickupFailure() const
{
    return signal_pickup_failure_;
}

types::DegreesFahrenheit SoftwareTrainController::GetCommandedInternalTemperature() const
{
    return commanded_internal_temperature_;
}

types::DegreesFahrenheit SoftwareTrainController::GetActualInternalTemperature() const
{
    return actual_internal_temperature_;
}

types::MilesPerHour SoftwareTrainController::GetCommandedSpeed() const
{
    return convert::MetersPerSecondToMilesPerHour(commanded_speed_);
}

types::MilesPerHour SoftwareTrainController::GetCurrentSpeed() const
{
    return convert::MetersPerSecondToMilesPerHour(current_speed_);
}

types::Blocks SoftwareTrainController::GetAuthority() const
{
    return authority_;
}

bool SoftwareTrainController::GetOperationMode() const
{
    return operation_mode_;
}

// Setters
void SoftwareTrainController::SetCommandedSpeed(const types::MetersPerSecond speed)
{
    commanded_speed_ = speed;
}

void SoftwareTrainController::SetDriverSpeed(const types::MilesPerHour speed)
{
    driver_speed_ = convert::MilesPerHourToMetersPerSecond(speed);
}

void SoftwareTrainController::SetCurrentSpeed(const types::MetersPerSecond speed)
{
    current_speed_ = speed;
}

void SoftwareTrainController::SetServiceBrake(const double percentage)
{
    service_brake_percentage_ = percentage;
}

void SoftwareTrainController::SetEmergencyBrake(const bool state)
{
    emergency_brake_ = state;
}

void SoftwareTrainController::SetHeadLights(const bool state)
{
    headlights_ = state;
}

void SoftwareTrainController::SetInteriorLights(const bool state)
{
    interior_lights_ = state;
}

void SoftwareTrainController::SetLeftDoors(const bool state)
{
    left_door_ = state;
}

void SoftwareTrainController::SetRightDoors(const bool state)
{
    right_door_ = state;
}

void SoftwareTrainController::SetBrakeFailure(const bool state)
{
    brake_failure_ = state;
}

void SoftwareTrainController::SetSignalPickupFailure(const bool state)
{
    signal_pickup_failure_ = state;
}

void SoftwareTrainController::SetEngineFailure(const bool state)
{
    engine_failure_ = state;
}

void SoftwareTrainController::SetCommandedInternalTemperature(const types::DegreesFahrenheit temp)
{
    commanded_internal_temperature_ = temp;
}

void SoftwareTrainController::SetActualInternalTemperature(const types::DegreesFahrenheit temp)
{
    actual_internal_temperature_ = temp;
}

void SoftwareTrainController::SetAuthority(const types::Blocks authority)
{
    authority_ = authority;
}

void SoftwareTrainController::SetKP(const uint16_t kp)
{
    if (distance_travelled_ == 0)
    {
        kp_ = kp;
    }
}
void SoftwareTrainController::SetKI(const uint16_t ki)
{
    if (distance_travelled_ == 0)
    {
        ki_ = ki;
    }
}

void SoftwareTrainController::SetArrived(const bool arrived)
{
    arrived_ = arrived;
}

void SoftwareTrainController::SetOperationMode(const bool mode)
{
    if (operation_mode_ == false && mode == true)
    {
        driver_speed_   = commanded_speed_;
        operation_mode_ = mode; // cannot switch back to auto
    }
}

void SoftwareTrainController::SetPolartity(const types::Polarity polarity)
{
    polarity_ = polarity;
}




void SoftwareTrainController::Update()
{
    std::chrono::milliseconds elapsed_time = (*clock_).GetElapsedTime(last_tick_updated_);

    types::Second delta_time = std::chrono::duration_cast<types::Second>(elapsed_time);

    last_tick_updated_ = (*clock_).GetTick();

    UpdateTrainPosition();
    CalculateDistanceToStopping();
    UpdateDistanceTravelled(delta_time);
    CalculateCommandedPower(delta_time);


    delta_time_ = delta_time;
}


void SoftwareTrainController::CalculateCommandedPower(const types::Second delta_time)
{
    // P(t) = Kp*[V_cmd(t) - v(t)]  +  Ki*∫[Vcmd(τ) - ActualSpeed(τ)]dτ
    // A function in time that represents the PI Controller

    types::KilometersPerHour block_speed_limit_temp = (green_block_data_map_[green_default_route_vector_[set_route_position_]])[2];
    types::MetersPerSecond   block_speed_limit      = convert::KilometersPerHourToMetersPerSecond(block_speed_limit_temp);

    types::MetersPerSecond setpoint_speed;

    if (commanded_speed_ > block_speed_limit)
    {
        commanded_speed_ = block_speed_limit;
    }

    if (driver_speed_ > block_speed_limit)
    {
        driver_speed_ = block_speed_limit;
    }
    else if (driver_speed_ < 0)
    {
        driver_speed_ = 0;
    }

    // Defining Vcmd and Actual speed in m/s
    if (operation_mode_ == false) // automatic
    {
        setpoint_speed = commanded_speed_;
    }
    else // manual
    {
        setpoint_speed = driver_speed_;
    }

    // Calculating speed_error
    types::MetersPerSecond speed_error = setpoint_speed - current_speed_;

    // Calculating Kp term
    double kp_term = speed_error * kp_;


    // This section is where the integral section of the equation will be calculated
    integral_sum_ += speed_error * delta_time.count();

    // Calculating Ki term
    double ki_term = ki_ * integral_sum_;

    CheckFailureStates();

    types::Meters distance_to_start_slowing_down = distance_of_authority_in_meters_ - 236.196;

    if (distance_to_start_slowing_down < 0)
    {
        distance_to_start_slowing_down = 0;
    }


    // std::cout << "\n" << distance_to_start_slowing_down << " A\n";
    // std::cout << "\n" << distance_travelled_ - distance_prior_to_current_authority_ << " B\n";

    //checking if emergency brake is active.
    if (emergency_brake_ == true)
    {
        integral_sum_             = 0;
        commanded_power_          = 0;
        service_brake_percentage_ =  0;
    }

    // Checking if we've reached a distance to start slowing down for authority
    else if (((distance_travelled_ - distance_prior_to_current_authority_) > distance_to_start_slowing_down))
    {
        if (new_authority_)
        {
            // 0 = current_speed^2 + 2*a*((distance_of_authority_in_meters - (distance_travelled_ - distance_prior_to_current_authority_))
            types::MetersPerSecondSquared required_acceleration = ((current_speed_ * current_speed_)) / (2 * ((distance_of_authority_in_meters_ - (distance_travelled_ - distance_prior_to_current_authority_))));

            if (required_acceleration / MAXIMUM_DECELERATION > 1 || required_acceleration < 0)
            {
                service_brake_percentage_ = 1;
                commanded_power_          = 0;
            }
            else
            {
                service_brake_percentage_ = required_acceleration / MAXIMUM_DECELERATION;
                commanded_power_          = 0;
            }

            new_authority_ = false;
        }
    }

    //Checking if Current Train Velocity is greater than Setpoint speed
    else if (current_speed_ > setpoint_speed)
    {
        integral_sum_ = 0;

        commanded_power_ = 0;

        types::MetersPerSecond speed_difference = current_speed_ - setpoint_speed;

        //Function to to assign service brake
        CalculateServiceBrake(speed_difference);
    }

    //Checking if Service brake is on
    else if (service_brake_percentage_ > 0)
    {
        integral_sum_    = 0;
        commanded_power_ = 0;
    }

    //Normal power calculation
    else
    {
        commanded_power_ = kp_term + ki_term;

        if (commanded_power_ > max_power_)
        {
            commanded_power_ = max_power_;
        }
    }
}

void SoftwareTrainController::CalculateServiceBrake(types::MetersPerSecond speed_difference)
{
    types::MetersPerSecond maximum_speed = convert::KilometersPerHourToMetersPerSecond(train_max_speed_);
    //Bins to increment service brake percentage by 10%

    //TODO - NNF-184: Rework this if-else statement into a single calculation
    if ((speed_difference > 0)  && (speed_difference <= (maximum_speed * 0.1)))
    {
        service_brake_percentage_ = 0.1;
    }
    else if ((speed_difference > maximum_speed * 0.1) && (speed_difference <= (maximum_speed * 0.2)))
    {
        service_brake_percentage_ = 0.2;
    }
    else if ((speed_difference > maximum_speed * 0.2)  && (speed_difference <= (maximum_speed * 0.3)))
    {
        service_brake_percentage_ = 0.3;
    }
    else if ((speed_difference > maximum_speed * 0.3) && (speed_difference <= (maximum_speed * 0.4)))
    {
        service_brake_percentage_ = 0.4;
    }
    else if ((speed_difference > maximum_speed * 0.4) && (speed_difference <= (maximum_speed * 0.5)))
    {
        service_brake_percentage_ = 0.5;
    }
    else if ((speed_difference > maximum_speed * 0.5) && (speed_difference <= (maximum_speed * 0.6)))
    {
        service_brake_percentage_ = 0.6;
    }
    else if ((speed_difference > maximum_speed * 0.6) && (speed_difference <= (maximum_speed * 0.7)))
    {
        service_brake_percentage_ = 0.7;
    }
    else if ((speed_difference > maximum_speed * 0.7) && (speed_difference <= (maximum_speed * 0.8)))
    {
        service_brake_percentage_ = 0.8;
    }
    else if ((speed_difference > maximum_speed * 0.8) && (speed_difference <= (maximum_speed * 0.9)))
    {
        service_brake_percentage_ = 0.9;
    }
    else if (speed_difference > (maximum_speed * 0.9))
    {
        service_brake_percentage_ = 1;
    }
}

void SoftwareTrainController::UpdateDistanceTravelled(const types::Second delta_time)
{
    types::Meters last_distance_travelled = distance_travelled_;
    distance_travelled_ += current_speed_ * delta_time.count();
    distance_since_last_update_ = distance_travelled_ - last_distance_travelled;
}

types::Second SoftwareTrainController::GetDeltaTime(void) const
{
    return delta_time_;
}

void SoftwareTrainController::CheckFailureStates(void)
{
    if ((engine_failure_ == true) || (signal_pickup_failure_ == true) || (brake_failure_ == true))
    {
        emergency_brake_ = true;
    }
}

void SoftwareTrainController::UpdateTrainPosition(void)
{
    if (last_polarity_ != polarity_)
    {
        set_route_position_++;
        authority_counter_--;
        double block_length = (green_block_data_map_[green_default_route_vector_[set_route_position_]])[0];


        last_polarity_ = polarity_;

        total_blocks_accessed_length_ += block_length;

        if (set_route_position_ > green_default_route_vector_.size() - 1)
        {
            set_route_position_ = 0; //to beggining of route
        }
    }
}

void SoftwareTrainController::CalculateDistanceToStopping()
{
    // std::cout <<  "\n AC: " << authority_counter_ << "\n";
    // std::cout <<  "\n A: " << authority_ << "\n";
    if (authority_counter_ != authority_)
    {
        usable_authority_         = authority_;
        authority_counter_        = authority_;
        new_authority_            = true;
        service_brake_percentage_ = 0; // Resetting the service brake when a new authority is passed through.

        distance_prior_to_current_authority_ = distance_travelled_;
        distance_of_authority_in_meters_     = 0;

        for (size_t i = set_route_position_ + 1; i < set_route_position_ + usable_authority_ + 1; i++)
        {
            int    index        = i % green_default_route_vector_.size();
            double block_length = (green_block_data_map_[green_default_route_vector_[index]])[0];

            if (index == set_route_position_ + usable_authority_)
            {
                distance_of_authority_in_meters_ += block_length / 2;
                distance_of_authority_in_meters_ += total_blocks_accessed_length_ - distance_prior_to_current_authority_;
            }
            else
            {
                distance_of_authority_in_meters_ += block_length;
            }
        }
    }


}


types::Meters SoftwareTrainController::GetDistanceOfAuthorityInMeters()
{
    return distance_of_authority_in_meters_;
}

}