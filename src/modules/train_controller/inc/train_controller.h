/*****************************************************************************
* @file train_controller.h
*
* @brief Implements the train controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H
#define TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H


#include "types.h"
#include <string>
#include <cstdint>

namespace train_controller
{

class TrainController
{
    public:
        uint8_t Kp = 2;
        uint8_t Ki = 4;



        virtual types::Watts CalculateCommandedPower(uint8_t speed);
        virtual types::Watts GetBrake(uint8_t percent); // ask if service brake is one time application and goes back to 0 or if it is continuous until set back to 0
        virtual void EmergencyBrake(void);
        //Functions that will output from track controller
        virtual bool ToggleHeadLights(void);
        virtual bool ToggleInteriorLights(void);
        virtual bool ToggleLeftDoors(void);
        virtual bool ToggleRightDoors(void);
        virtual std::string GetStationAnnouncement(void) const;
        virtual types::Meters GetDistanceTravelled(void) const;
        virtual void SetInternalTemperature(types::DegreesFahrenheit temp);
        virtual std::uint16_t GetInternalTemperature(void) const;

        // inputs from train model
        // virtual types::MetersPerSecond GetCommandedSpeed(void) const;
        // virtual types::MetersPerSecond GetActualSpeed(void) const;
        // virtual std::size_t GetBeaconData(types::BeaconData &data) const;
        // virtual types::Meters GetAuthority(void) const;
        // virtual types::Watts GetActualPower(void) const;
        // virtual bool GetEngineFailure(void) const;
        // virtual bool GetSignalPickupFailure(void) const;
        // virtual bool GetBrakeFailure(void) const;
        
        // inputs from driver
        virtual void SetDriverSpeed(uint16_t);
        virtual void SetCommandedInternalTemperature(uint16_t);
        virtual void SetServiceBrake(uint16_t);
        virtual void setEmergencyBrake(bool);

        // inputs from engineer
        virtual void SetKP(uint16_t);
        virtual void setKI(uint16_t);



        //TestBench Functions. These will be tied to the testbench UI
        virtual types::MetersPerSecond SetCommandedSpeedTB(void) const;
        virtual types::MetersPerSecond SetActualSpeedTB(void) const;
        virtual std::size_t SetBeaconDataTB(types::BeaconData &data) const;
        virtual types::Meters SetAuthorityTB(void) const;
        virtual types::Watts SetActualPowerTB(void) const;
        virtual bool SetEngineFailureTB(void) const;
        virtual bool SetSignalPickupFailureTB(void) const;
        virtual bool SetBrakeFailureTB(void) const;


};

} // namespace train_controller

#endif // TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H