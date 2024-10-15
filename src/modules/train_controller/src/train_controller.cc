#include "train_controller.h"
#include "types.h"
#include "convert.h"

namespace train_controller
{
// Getters
double SoftwareTrainController::GetServiceBrake() const
{
    return service_brake_percentage_;
}

double SoftwareTrainController::GetGrade(void) const
{
    return 0;
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

types::Meters SoftwareTrainController::GetAuthority() const
{
    return authority_;
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

void SoftwareTrainController::SetAuthority(const types::Meters authority)
{
    authority_ = authority;
}

void SoftwareTrainController::SetKP(const uint16_t kp)
{
    kp_ = kp;
}
void SoftwareTrainController::setKI(const uint16_t ki)
{
    ki_ = ki;
}

void SoftwareTrainController::SetArrived(const bool arrived)
{
    arrived_ = arrived;
}

void SoftwareTrainController::CalculateCommandedPower()
{
    // P(t) = Kp*[V_cmd(t) - v(t)]  +  Ki*∫[Vcmd(τ) - ActualSpeed(τ)]dτ
    // A function in time that represents the PI Controller

    float block_speed_limit = 50.0; // TODO - Add hashmap with track data

    // Defining Vcmd and Actual speed in m/s
    types::MetersPerSecond setpoint_speed = driver_speed_;

    // TODO - NNF-182

    // convert to m/s from km/hr
    block_speed_limit = convert::KilometersPerHourToMetersPerSecond(block_speed_limit);

    if (setpoint_speed > block_speed_limit)
    {
        setpoint_speed = block_speed_limit;
    }

    // Calculating speed_error
    double speed_error = setpoint_speed - current_speed_;

    // Calculating Kp term
    float kp_term = speed_error * kp_;

    // Temp time passed since last update
    float delta_time = 1; // TODO - NNF-181

    // This section is where the integral section of the equation will be calculated
    integral_sum_ += speed_error * delta_time;

    // Calculating Ki term
    float ki_term = ki_ * integral_sum_;


    if ((emergency_brake_ == true) || arrived_)
    {
        integral_sum_    = 0;
        commanded_power_ = 0;
    }

    //Checking if Current Train Velocity is greater than Setpoint speed
    else
    if (current_speed_ > driver_speed_)
    {
        integral_sum_    = 0;
        commanded_power_ = 0;

        types::MetersPerSecond speed_difference = current_speed_ - driver_speed_;

        //Function to to assign service brake
        CalculateServiceBrake(speed_difference);
    }
    //Checking if Service brake is on
    else
    if (service_brake_percentage_ > 0)
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

void SoftwareTrainController::CalculateServiceBrake(double speed_difference)
{
    types::MetersPerSecond maximum_speed = convert::KilometersPerHourToMetersPerSecond(train_max_speed_);
    //Bins to increment service brake percentage by 10%

    // speed diff <= train_max_mps*0.1

    if ((speed_difference > 0)  && (speed_difference <= maximum_speed * 0.1))
    {
        service_brake_percentage_ = 10;
    }
    else
    if ((speed_difference > maximum_speed * 0.1) && (speed_difference <= maximum_speed * 0.2))
    {
        service_brake_percentage_ = 20;
    }
    else
    if ((speed_difference > maximum_speed * 0.2)  && (speed_difference <= maximum_speed * 0.3))
    {
        service_brake_percentage_ = 30;
    }
    else
    if ((speed_difference > maximum_speed * 0.3) && (speed_difference <= maximum_speed * 0.4))
    {
        service_brake_percentage_ = 40;
    }
    else
    if ((speed_difference > maximum_speed * 0.4) && (speed_difference <= maximum_speed * 0.5))
    {
        service_brake_percentage_ = 50;
    }
    else
    if ((speed_difference > maximum_speed * 0.5) && (speed_difference <= maximum_speed * 0.6))
    {
        service_brake_percentage_ = 60;
    }
    else
    if ((speed_difference > maximum_speed * 0.6) && (speed_difference <= maximum_speed * 0.7))
    {
        service_brake_percentage_ = 70;
    }
    else
    if ((speed_difference > maximum_speed * 0.7) && (speed_difference <= maximum_speed * 0.8))
    {
        service_brake_percentage_ = 80;
    }
    else
    if ((speed_difference > maximum_speed * 0.8) && (speed_difference <= maximum_speed * 0.9))
    {
        service_brake_percentage_ = 90;
    }
    else
    if (speed_difference > maximum_speed * 0.9)
    {
        service_brake_percentage_ = 100;
    }
}

void SoftwareTrainController::UpdateDistanceTravelled(long interval)
{
    distance_travelled_ += current_speed_ * interval;
}
}