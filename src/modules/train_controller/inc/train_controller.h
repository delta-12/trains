/*****************************************************************************
* @file train_controller.h
*
* @brief Implements the train controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H
#define TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H

#include <array>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "types.h"

namespace train_controller
{

class TrainController
{
    public:
        //Getters
        virtual types::MetersPerSecond GetCurrentSpeed(void) const                   = 0;
        virtual types::MilesPerHour GetDriverSpeed(void) const                       = 0;
        virtual types::MetersPerSecond GetCommandedSpeed(void) const                 = 0;
        virtual types::Miles GetDistanceTravelled(void) const                        = 0;
        virtual types::Watts GetCommandedPower(void) const                           = 0;
        virtual double GetServiceBrake(void) const                                   = 0;
        virtual double GetGrade(void) const                                          = 0;
        virtual bool GetEmergencyBrake(void) const                                   = 0;
        virtual bool GetHeadLights(void) const                                       = 0;
        virtual bool GetInteriorLights(void) const                                   = 0;
        virtual bool GetLeftDoors(void) const                                        = 0;
        virtual bool GetRightDoors(void) const                                       = 0;
        virtual bool GetBrakeFailure(void) const                                     = 0;
        virtual bool GetSignalPickupFailure(void) const                              = 0;
        virtual bool GetEngineFailure(void) const                                    = 0;
        virtual types::DegreesFahrenheit GetCommandedInternalTemperature(void) const = 0;
        virtual types::DegreesFahrenheit GetActualInternalTemperature(void) const    = 0;
        virtual types::Meters GetAuthority(void) const                               = 0;

        // Setter declarations
        virtual void SetCommandedSpeed(const types::MetersPerSecond speed)                = 0;
        virtual void SetDriverSpeed(const types::MilesPerHour speed)                      = 0;
        virtual void SetCurrentSpeed(const types::MetersPerSecond speed)                  = 0;
        virtual void SetServiceBrake(const double percentage)                             = 0;
        virtual void setEmergencyBrake(const bool state)                                  = 0;
        virtual void SetHeadLights(const bool state)                                      = 0;
        virtual void SetInteriorLights(const bool state)                                  = 0;
        virtual void SetLeftDoors(const bool state)                                       = 0;
        virtual void SetRightDoors(const bool state)                                      = 0;
        virtual void SetBrakeFailure(const bool state)                                    = 0;
        virtual void SetSignalPickupFailure(const bool state)                             = 0;
        virtual void SetEngineFailure(const bool state)                                   = 0;
        virtual void SetCommandedInternalTemperature(const types::DegreesFahrenheit temp) = 0;
        virtual void SetActualInternalTemperature(const types::DegreesFahrenheit temp)    = 0;
        virtual void SetAuthority(const types::Meters authority)                          = 0;
        virtual void SetKP(const uint16_t kp)                                             = 0;
        virtual void SetKI(const uint16_t ki)                                             = 0;
};


class SoftwareTrainController : public TrainController
{
    public:
        // Implementations for getters
        types::MetersPerSecond GetCurrentSpeed(void) const;
        types::MilesPerHour GetDriverSpeed(void) const;
        types::MetersPerSecond GetCommandedSpeed(void) const;
        types::Meters GetDistanceTravelled(void) const;
        types::Watts GetCommandedPower(void) const;
        double GetServiceBrake(void) const;
        double GetGrade(void) const;
        bool GetEmergencyBrake(void) const;
        bool GetHeadLights(void) const;
        bool GetInteriorLights(void) const;
        bool GetLeftDoors(void) const;
        bool GetRightDoors(void) const;
        bool GetBrakeFailure(void) const;
        bool GetSignalPickupFailure(void) const;
        bool GetEngineFailure(void) const;
        types::DegreesFahrenheit GetCommandedInternalTemperature(void) const;
        types::DegreesFahrenheit GetActualInternalTemperature(void) const;
        types::Meters GetAuthority(void) const;

        // Implementations for setters
        void SetCommandedSpeed(const types::MetersPerSecond speed);
        void SetDriverSpeed(const types::MilesPerHour speed);
        void SetCurrentSpeed(const types::MetersPerSecond speed);
        void SetServiceBrake(const double percentage);
        void setEmergencyBrake(const bool state);
        void SetHeadLights(const bool state);
        void SetInteriorLights(const bool state);
        void SetLeftDoors(const bool state);
        void SetRightDoors(const bool state);
        void SetBrakeFailure(const bool state);
        void SetSignalPickupFailure(const bool state);
        void SetEngineFailure(const bool state);
        void SetCommandedInternalTemperature(const types::DegreesFahrenheit temp);
        void SetActualInternalTemperature(const types::DegreesFahrenheit temp);
        void SetAuthority(const types::Meters authority);
        void SetArrived(const bool arrived);
        void SetKP(const uint16_t ki);
        void setKI(const uint16_t kp);

        //local functions
        void CalculateCommandedPower();
        void UpdateDistanceTravelled(long interval);
        void CalculateServiceBrake(double speed_difference);

    private:
        float integral_sum_ = 0;
        float kp_           = 4;
        float ki_           = 2;

        types::KilometersPerHour train_max_speed_ = 70;
        types::Watts max_power_                   = 120000;
        types::MetersPerSecond commanded_speed_   = 0;
        types::MetersPerSecond driver_speed_;
        types::MetersPerSecond current_speed_ = 0;
        double service_brake_percentage_;
        types::Watts commanded_power_                            = 0;
        types::Meters authority_                                 = 0;
        bool emergency_brake_                                    = 0;
        bool headlights_                                         = 0;
        bool interior_lights_                                    = 0;
        bool left_door_                                          = 0;
        bool right_door_                                         = 0;
        bool brake_failure_                                      = 0;
        bool signal_pickup_failure_                              = 0;
        bool engine_failure_                                     = 0;
        types::DegreesFahrenheit commanded_internal_temperature_ = 68;
        types::DegreesFahrenheit actual_internal_temperature_    = 0;
        types::Meters distance_travelled_                        = 0;
        bool arrived_                                            = 0;
};

} // namespace train_controller

#endif // TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H