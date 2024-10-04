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
            

            //Functions that will output from track controller
            virtual types::Watts GetCommandedPower(float speed)                             =0;
            virtual types::Watts GetBrake(float percent)                                    =0;
            virtual void GetEmergencyBrake(void)                                            =0;
            virtual bool GetHeadLights(void)                                                =0;
            virtual bool GetInteriorLights(void)                                            =0;
            virtual bool GetLeftDoors(void)                                                 =0;
            virtual bool GetRightDoors(void)                                                =0;
            virtual std::string GetStationAnnouncement(void) const                          =0;
            virtual types::Meters GetDistanceTravelled(void) const                          =0;
            virtual void GetInternalTemperature(types::DegreesFahrenheit temp)              =0;
            virtual std::uint16_t GetInternalTemperature(void) const                        =0;

            // inputs from train model
            virtual types::MetersPerSecond SetCommandedSpeed(void) const                    =0;
            virtual types::MetersPerSecond SetActualSpeed(void) const                       =0;
            virtual std::size_t SetBeaconData(types::BeaconData &data) const                =0;
            virtual types::Meters SetAuthority(void) const                                  =0;
            virtual types::Watts SetActualPower(void) const                                 =0;
            virtual bool SetEngineFailure(void) const                                       =0;
            virtual bool SetSignalPickupFailure(void) const                                 =0;
            virtual bool SetBrakeFailure(void) const;
            
            // inputs from driver
            virtual void SetDriverSpeed(uint16_t)                                           =0;
            virtual void SetCommandedInternalTemperature(uint16_t)                          =0;
            virtual void SetServiceBrake(uint16_t)                                          =0;
            virtual void setEmergencyBrake(bool)                                            =0;

            // inputs from engineer
            virtual void SetKP(uint16_t)                                                    =0;
            virtual void setKI(uint16_t)                                                    =0;
    };


    class SoftwareTrainController : public TrainController {
    public:
            uint8_t Kp = 2;
            uint8_t Ki = 4;

            //Functions that will output from track controller
            virtual types::Watts GetCommandedPower(float speed);
            virtual types::Watts GetBrake(float percent);
            virtual void GetEmergencyBrake(void);
            virtual bool GetHeadLights(void);
            virtual bool GetInteriorLights(void);
            virtual bool GetLeftDoors(void);
            virtual bool GetRightDoors(void);
            virtual std::string GetStationAnnouncement(void) const;
            virtual types::Meters GetDistanceTravelled(void) const;
            virtual void GetInternalTemperature(types::DegreesFahrenheit temp);
            virtual std::uint16_t GetInternalTemperature(void) const;

            // inputs from train model
            virtual types::MetersPerSecond SetCommandedSpeed(void) const;
            virtual types::MetersPerSecond SetActualSpeed(void) const;
            virtual std::size_t SetBeaconData(types::BeaconData &data) const;
            virtual types::Meters SetAuthority(void) const;
            virtual types::Watts SetActualPower(void) const;
            virtual bool SetEngineFailure(void) const;
            virtual bool SetSignalPickupFailure(void) const;
            virtual bool SetBrakeFailure(void) const;
            
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