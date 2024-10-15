/*****************************************************************************
* @file train_controller.h
*
* @brief Implements the train controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H
#define TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H


#define ZERO (0)
#define TRAIN_CONTROLLER_DEFAULT_KP (4)
#define TRAIN_CONTROLLER_DEFAULT_KI (2)
#define TRAIN_CONTROLLER_MAXIMUM_ENGINE_POWER (120000)
#define TRAIN_SPEED_LIMIT (120000)
#define DEFAULT_BLOCK_SPEED_LIMIT (50)
#define DEFAULT_BLOCK_GRADE (0)
#define DEFAULT_TRAIN_TEMPERATURE (68)
#define DEFAULT_DELTA_TIME (1)


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
        virtual void SetCommandedSpeed(const types::MetersPerSecond speed)                       = 0;
        virtual void SetDriverSpeed(const types::MilesPerHour speed)                             = 0;
        virtual void SetCurrentSpeed(const types::MetersPerSecond speed)                         = 0;
        virtual void SetServiceBrake(const double percentage)                                    = 0;
        virtual void SetEmergencyBrake(const bool state)                                         = 0;
        virtual void SetHeadLights(const bool state)                                             = 0;
        virtual void SetInteriorLights(const bool state)                                         = 0;
        virtual void SetLeftDoors(const bool state)                                              = 0;
        virtual void SetRightDoors(const bool state)                                             = 0;
        virtual void SetBrakeFailure(const bool state)                                           = 0;
        virtual void SetSignalPickupFailure(const bool state)                                    = 0;
        virtual void SetEngineFailure(const bool state)                                          = 0;
        virtual void SetCommandedInternalTemperature(const types::DegreesFahrenheit temperature) = 0;
        virtual void SetActualInternalTemperature(const types::DegreesFahrenheit temperature)    = 0;
        virtual void SetAuthority(const types::Meters authority)                                 = 0;
        virtual void SetKP(const uint16_t kp)                                                    = 0;
        virtual void SetKI(const uint16_t ki)                                                    = 0;
};


class SoftwareTrainController : public TrainController
{
    public:
        // Constructor
        SoftwareTrainController();

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
        void SetEmergencyBrake(const bool state);
        void SetHeadLights(const bool state);
        void SetInteriorLights(const bool state);
        void SetLeftDoors(const bool state);
        void SetRightDoors(const bool state);
        void SetBrakeFailure(const bool state);
        void SetSignalPickupFailure(const bool state);
        void SetEngineFailure(const bool state);
        void SetCommandedInternalTemperature(const types::DegreesFahrenheit temperature);
        void SetActualInternalTemperature(const types::DegreesFahrenheit temperature);
        void SetAuthority(const types::Meters authority);
        void SetArrived(const bool arrived);
        void SetKP(const uint16_t ki);
        void setKI(const uint16_t kp);

        //local functions
        void CalculateCommandedPower(void);
        void UpdateDistanceTravelled(long interval); // NNF-181 TODO: Update the interval application to make use of Tick Source
        void CalculateServiceBrake(types::MetersPerSecond speed_difference);

    private:

        float integral_sum_;
        uint16_t kp_;
        uint16_t ki_;

        types::KilometersPerHour train_max_speed_;
        types::Watts max_power_;
        types::MetersPerSecond commanded_speed_;
        types::MetersPerSecond driver_speed_;
        types::MetersPerSecond current_speed_;
        double service_brake_percentage_;
        types::Watts commanded_power_;
        types::Meters authority_;
        bool emergency_brake_;
        bool headlights_;
        bool interior_lights_;
        bool left_door_;
        bool right_door_;
        bool brake_failure_;
        bool signal_pickup_failure_;
        bool engine_failure_;
        types::DegreesFahrenheit commanded_internal_temperature_;
        types::DegreesFahrenheit actual_internal_temperature_;
        types::Meters distance_travelled_;
        bool arrived_;
};

} // namespace train_controller

#endif // TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H